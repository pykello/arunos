#include <block.h>
#include <fat.h>
#include <lib/math.h>
#include <lib/string.h>

static uint32_t fat_start, root_start, data_start, clusters;
static uint32_t cluster_bytes;
static uint16_t root_entries;
static uint8_t sectors_per_cluster;
static bool mounted;

static uint16_t le16(const uint8_t *p)
{
	return p[0] | (p[1] << 8);
}

static uint32_t le32(const uint8_t *p)
{
	return le16(p) | ((uint32_t)le16(p + 2) << 16);
}

static bool valid_cluster(uint32_t cluster)
{
	return cluster >= 2 && cluster < clusters + 2 && cluster < 0xfff0;
}

static bool next_cluster(uint16_t cluster, uint16_t *next)
{
	uint8_t sector[512];

	if (!valid_cluster(cluster) ||
	    !block_read(fat_start + cluster / 256, sector))
		return false;
	*next = le16(sector + (cluster % 256) * 2);
	return true;
}

bool fat_init(void)
{
	uint8_t b[512];
	uint32_t total, fat_sectors, root_sectors;

	mounted = false;
	if (!block_read(0, b) || le16(b + 510) != 0xaa55 ||
	    le16(b + 11) != 512)
		return false;
	sectors_per_cluster = b[13];
	fat_start = le16(b + 14);
	root_entries = le16(b + 17);
	fat_sectors = le16(b + 22);
	total = le16(b + 19);
	if (!total)
		total = le32(b + 32);
	if (!sectors_per_cluster || sectors_per_cluster > 128 ||
	    (sectors_per_cluster & (sectors_per_cluster - 1)) ||
	    !fat_start || !root_entries || !fat_sectors ||
	    (b[16] != 1 && b[16] != 2) || total > block_sector_count())
		return false;
	root_sectors = (root_entries * 32 + 511) / 512;
	root_start = fat_start + b[16] * fat_sectors;
	data_start = root_start + root_sectors;
	if (total <= data_start)
		return false;
	clusters = unsigned_divmod(total - data_start, sectors_per_cluster,
				  NULL);
	if (clusters < 4085 || clusters >= 65525 ||
	    clusters + 2 > fat_sectors * 256)
		return false;
	cluster_bytes = sectors_per_cluster * 512;
	mounted = true;
	return true;
}

/* Convert a plain ASCII 8.3 filename to its on-disk padded form. */
static bool short_name(const char *name, char key[11])
{
	uint32_t i = 0, pos = 0, limit = 8;

	if (!name || !name[0])
		return false;
	memset(key, ' ', 11);
	for (; name[i]; i++) {
		unsigned char ch = name[i];
		if (ch == '.') {
			if (!pos || limit == 11 || !name[i + 1])
				return false;
			pos = 8;
			limit = 11;
			continue;
		}
		if (pos == limit || ch <= ' ' || ch >= 127 ||
		    strchr("\"*+,/:;<=>?[\\]|", ch))
			return false;
		if (ch >= 'a' && ch <= 'z')
			ch -= 'a' - 'A';
		key[pos++] = ch;
	}
	return true;
}

bool fat_open(const char *name, struct FatFile *file)
{
	uint8_t sector[512];
	char key[11];
	uint32_t i, j, count;
	uint16_t cluster;

	if (!mounted || !short_name(name, key))
		return false;
	for (i = 0; i < root_entries; i++) {
		uint8_t *entry = sector + (i % 16) * 32;
		if (i % 16 == 0 && !block_read(root_start + i / 16, sector))
			return false;
		if (!entry[0])
			return false;
		if (entry[0] == 0xe5 || (entry[11] & 0x18))
			continue;
		for (j = 0; j < 11 && entry[j] == (uint8_t)key[j]; j++);
		if (j != 11)
			continue;
		file->size = le32(entry + 28);
		file->cluster = le16(entry + 26);
		if (!file->size)
			return file->cluster == 0;
		count = unsigned_divmod(file->size - 1, cluster_bytes,
					NULL) + 1;
		if (count > clusters)
			return false;
		/* Validate the whole chain, including cycles and early EOF. */
		cluster = file->cluster;
		for (j = 0; j < count; j++)
			if (!next_cluster(cluster, &cluster))
				return false;
		return cluster >= 0xfff8;
	}
	return false;
}

bool fat_read(const struct FatFile *file, uint32_t offset, void *buffer,
	      uint32_t length)
{
	uint8_t sector[512], *out = buffer;
	uint16_t cluster = file->cluster;
	uint32_t skip, within, n;

	if (!mounted || offset > file->size || length > file->size - offset)
		return false;
	if (!length)
		return true;
	skip = unsigned_divmod(offset, cluster_bytes, &within);
	if (skip >= clusters)
		return false;
	while (skip--)
		if (!next_cluster(cluster, &cluster))
			return false;
	while (length) {
		if (!valid_cluster(cluster) ||
		    !block_read(data_start + (cluster - 2) *
				sectors_per_cluster + within / 512, sector))
			return false;
		n = 512 - within % 512;
		if (n > length)
			n = length;
		memcpy(out, sector + within % 512, n);
		out += n;
		length -= n;
		within += n;
		if (within == cluster_bytes && length) {
			if (!next_cluster(cluster, &cluster))
				return false;
			within = 0;
		}
	}
	return true;
}
