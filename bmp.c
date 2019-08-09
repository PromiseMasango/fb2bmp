#include <stdio.h>
#include "bmp.h"

/*function declaration*/
void writeHeader(FILE *file , struct BMPFileHeader *h);
void writeBitmap(FILE *file ,struct BMPBitmapHeader *h);

/*file information*/
void writeHeader(FILE *file , struct BMPFileHeader *h){
	fwrite( &h->type, 2 , 1 , file );
	fwrite( &h->size, 4 , 1 , file );
	fwrite( &h->res1, 2 , 1 , file );
	fwrite( &h->res2 ,2 , 1 , file );
	fwrite( &h->offset , 4 , 1 , file );
}

/*bitmap information*/
void writeBitmap(FILE *file ,struct BMPBitmapHeader *h){
	fwrite( &h->size , 4, 1, file );
	fwrite( &h->width , 4 , 1 , file );
	fwrite( &h->height , 4 , 1 , file );
	fwrite( &h->planes , 2 , 1 , file );
	fwrite( &h->bpp , 2 , 1 , file );
	fwrite( &h->comp , 4 , 1 , file );
	fwrite( &h->bitmap , 4 , 1 , file );
	fwrite( &h->hres , 4 , 1 , file );
	fwrite( &h->vres , 4 , 1 , file );
	fwrite( &h->colorUsed ,4 , 1 , file );
	fwrite( &h->icolor , 4 , 1 , file );
}

