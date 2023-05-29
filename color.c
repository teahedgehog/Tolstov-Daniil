#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>
#include "lodepng.h"

unsigned char * load_PNG(const char* filename, unsigned * width, unsigned * height);
void write_PNG(const char* filename, unsigned char* image, unsigned width, unsigned height);

unsigned char * load_PNG(const char* filename, unsigned * width, unsigned * height) {
	unsigned char * image = NULL;
	unsigned error = lodepng_decode32_file(&image, width, height, filename);
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
	return image;
}

void write_PNG(const char* filename, unsigned char* image, unsigned width, unsigned height) {
	unsigned char * png;
	size_t pngsize;

	unsigned error = lodepng_encode32(&png, &pngsize, image, width, height);
	if(!error) lodepng_save_file(png, pngsize, filename);
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
	return;
}

int i, j;

void preparation( unsigned char *od, int ih, int iw ){
	for( i = 0; i < ih; i++ ){
		for( j = 0; j < iw; j++ ){
			if( od[iw * i + j] < 100 ) od[iw * i + j] = 0;
			if( od[iw * i + j] > 160 ) od[iw * i + j] = 255;
		}
	}
	return;
}

void fSobel( unsigned char *od, unsigned char *dc, int ih, int iw ){
	unsigned char x,  y;
	for( i = 2; i < ih - 1; i++ ){
		for( j = 2; j < iw -1; j++ ){
			x = (-1) * od[iw * ( i - 1 ) + ( j - 1 )] - 2 * od[iw * i + ( j - 1 )] - 1 * od[iw * (i + 1) + ( j - 1 )] + 1 * od[iw * ( i - 1 ) + ( j + 1 )] + 2 * od[iw * i + ( j + 1 )] + 1 * od[iw * ( i + 1 ) + ( j + 1 )];
			y = (-1) * od[iw * ( i - 1 ) + ( j - 1 )] - 2 * od[iw * ( i - 1 ) + j ] - 1 * od[iw * (i - 1) + ( j + 1 )] + 1 * od[iw * ( i + 1 ) + ( j - 1 )] + 2 * od[iw * ( i + 1 ) + j] + 1 * od[iw * ( i + 1 ) + ( j + 1 )];
			dc[iw * i + j] = sqrt( x * x + y * y ); 
		}
	}
}


void fGauss( unsigned char *od, unsigned char *dc, int ih, int iw ){
	for( i = 1; i < ih -1 ; i++ ){
		for(j = 1; j < iw - 1; j++ ){
		dc[iw * i + j ] += 0.12 * od[iw * i + j] + 0.12 * od[iw * ( i + 1 ) + j] + 0.12 * od[iw * ( i - 1 ) + j];
		dc[iw * i + j ] += od[iw * i + j] + 0.12 * od[iw * i + ( j + 1 )] + 0.12*od[iw * i + ( j - 1 )];
		dc[iw * i + j ] += od[iw * i + j] + 0.09 * od[iw * ( i + 1 ) + ( j + 1 )] + 0.09 * od[iw * ( i + 1 ) + ( j - 1 )];
		dc[iw * i + j ] += od[iw * i + j] + 0.09 * od[iw * ( i - 1 ) + ( j + 1 )] + 0.09 * od[iw * ( i - 1 ) + ( j - 1 )];
		}
	}
	return;
}

/*
void fGauss( unsigned char *od, unsigned char *dc, int ih, int iw ){
	double Gauss_mat[3][3] = {{0.09, 0.12, 0.09},
							  {0.12, 0.12, 0.12},
							  {0.09, 0.12, 0.09}};
	double c; 
	for(i = 1; i < ih-1; i++)
		for(j = 1; j < iw-1; j++){
			c = 0.0;
			for(int k = -1; k <= 1; k++){
				for(int w = -1; w <= 1; w++){
					c += Gauss_mat[k+1][w+1]*od[(i+k)*iw+(j+w)];
				}
			}
			dc[i*iw+j] = c;
		}
	return;
}
*/
void colouring( unsigned char * dc, unsigned char *mcod, int ih, int iw){
	for( i = 1; i < iw*ih; i++ ){
		mcod[4*i] = 90 + dc[i] + 0.7 * dc[i - 1];
		mcod[4*i + 1] = 20 + dc[i];
		mcod[4*i + 2] = 599 + dc[i];
		mcod[4*i + 3] = 200;
	}
	return ;
}

int main(){
	const char *inputPath = "skull.png";
	int iw, ih, k = 0;
  	unsigned char * idata = load_PNG(inputPath, &iw, &ih);
	if( idata == NULL ){
		printf( "ERROR: can't read file %s\n", inputPath );
		return 1;
	}
	unsigned char *odata = ( unsigned char* )malloc( ih * iw * sizeof( unsigned char ) );
	unsigned char *datacopy = ( unsigned char* )malloc( ih * iw * sizeof( unsigned char ) );
	unsigned char *mcodata = ( unsigned char* )malloc( 4*ih * iw * sizeof( unsigned char ) );
	for( i = 0; i < 4*ih * iw; i += 4 ){
		odata[k] = 0.321 * idata[i] + 0.231 * idata[i + 1] + 0.123 * idata[i + 2];
		k++;
	}
	preparation( odata, ih, iw );
	fGauss( odata, datacopy, ih, iw );
	colouring( datacopy, mcodata, ih, iw);
	const char *outputPath = "colouring_skull.png";
    write_PNG(outputPath, mcodata, iw, ih);
	return 0;
}