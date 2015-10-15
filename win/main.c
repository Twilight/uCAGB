#include <stdio.h>
#include <stdlib.h>

#include "pl_win.h"
#include "gba.h"

static const tSize align = 0x10;
const unsigned char *load_rom(const char *filename, tSize *psize){
	tSize size;
	FILE *f;
	unsigned char *data;
	f = fopen(filename, "rb");
	if(!f){
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	*psize = (size + align - 1) & (~(align - 1));
	data = malloc(*psize);
	memset(data, 0, *psize);
	fseek(f, 0, SEEK_SET);
	fread(data, 1, size, f);
	fclose(f);
	return data;
}

int main(int argc, const char *argv[]){
	const char *devname, *filename;
	tDev siodev;
	const char *rom;
	tSize size;
	uint t0, t1;

	if(argc != 3){
		fprintf(stderr, "invalid parameters, example:\n\t%s COM1 your_file.gba\n", argv[0]);
		return 0;
	}
	devname = argv[1]; filename = argv[2];

	siodev = open_serial(devname);
	if(!validate_serial(siodev)){
		fprintf(stderr, "can't open %s\n", devname);
		return 0;
	}

	rom = load_rom(filename, &size);
	if(rom == NULL){
		fprintf(stderr, "can't open %s\n", filename);
		return 0;
	}

	if(gba_ready(siodev)){
		fprintf(stderr, "\ndidn't get GBA ready signal before timeout\n");
		return 0;
	}

	t0 = get_rtime();
	gba_multiboot(siodev, rom, size);

	t1 = get_rtime();
	fprintf(stderr, "transfer time: %.2f seconds\n", (t1 - t0) / 1000.0);

	return 0;
}

