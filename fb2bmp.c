
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "bmp.c"

/* function declarations */
void die(char *err);
void usage();


void die(char *err){
	perror(err);
	exit(EXIT_FAILURE);
}

void usage(){
	printf("./fb2bmp <filename.bmp> <framebuffer device>\n");
	exit(1);
}

int main(int argv, char * argc[]){
	if (argv != 3 )
		usage();

	FILE *fout = fopen(argc[1], "wb");
	if( fout == NULL )
		die("error, cannot open file");

	/*structures*/
	struct BMPFileHeader *head = NULL;
	struct BMPBitmapHeader *bitmap = NULL;

	/*allocate memory*/
	head = malloc( sizeof( *head ) );
	bitmap = malloc( sizeof(*bitmap) );

	/*zero out memory*/
	memset(head, 0, sizeof(struct BMPFileHeader) );
	memset( bitmap, 0, sizeof(struct BMPBitmapHeader) );

	/*header information*/
	head->type = 0x4D42;
	head->size = 0;
	head->res1 = 0;
	head->res2 = 0;
	head->offset= 54;

	/*bitmap information*/
	bitmap->size = 40;
	bitmap->planes= 1;
	bitmap->bpp = 24;
	bitmap->comp  = 0;
	bitmap->bitmap = 0;
	bitmap->hres = 0;
	bitmap->vres = 0;
	bitmap->colorUsed = 0;
	bitmap->icolor= 0;

	/*open screen framebuffer*/
	int device = open(argc[2], O_RDWR);
	if( device == -1 )
		die("error, are you root?");

	/*define structures*/
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;

	/*zero out structures*/
	memset( &vinfo, 0, sizeof(vinfo));
	memset( &finfo, 0, sizeof(finfo));

	/*screen properties*/
	if ( ioctl(device, FBIOGET_FSCREENINFO, &finfo) == -1)
		die("FBIOGET_FSCREENINFO");

	if ( ioctl(device, FBIOGET_VSCREENINFO, &vinfo) == -1)
		die("FBIOGET_FSCREENINFO");

///////////////////////////////////////////
#ifdef DEBUG
	printf("id :%s\n", finfo.id);
	printf("resolution : %dx%d\n", vinfo.xres, vinfo.yres );
	printf("bits_per_pixel :%d\n", vinfo.bits_per_pixel);
#endif
///////////////////////////////////////////


	/*image width = screen width*/
	bitmap->width= vinfo.xres ;
	/*image height = screen height*/
	bitmap->height = abs(vinfo.yres);


	/*Write image header data*/
	writeHeader( fout , head );
	writeBitmap( fout , bitmap );

	/*mman*/
	char *buffer = NULL;
	long int screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	/*Memory map framebuffer*/
	buffer = (char *)mmap( NULL, screensize, PROT_READ , MAP_SHARED , device , 0);
	if( buffer == MAP_FAILED )
		die("MAP_FAILED");


	/*color channels*/
	unsigned char red = 0;
	unsigned char green = 0;
	unsigned char blue = 0;

	/*pixel location in framebuffer memory*/
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int location = 0;


	for( y = vinfo.yres; y > 0 ; y-- ){
		for( x = 0 ; x < vinfo.xres; x++ ){
			location = (x+vinfo.xoffset) *  (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;

			/*read pixels from framebuffer device*/
			red  = *(buffer + location );
			green= *(buffer + location + 1 );
			blue = *(buffer + location + 2 );

			/*write to file*/
			fwrite( &red, 1, 1, fout );
			fwrite( &green, 1, 1, fout );
			fwrite( &blue, 1, 1, fout );
		}
	}

	/*unmap mapped memory*/
	munmap( buffer, screensize );

	/*free allocated memory*/
	free(head);
	free(bitmap);

	/*close file descriptors*/
	close( device );
	fclose(fout);

	return 0;
}

