#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PKT_SIZE (640 * 480 * 4)

using namespace std;

// comment to send pixels as commands via regular write function of char driver
// leave uncommented to write directly to memory (faster)

int Boja(std::string boja) {
  int boja_vr = 0;

  if (boja.compare("RED") == 0)
    boja_vr = 0xF800;
  else if (boja.compare("GREEN") == 0)
    boja_vr = 0x07E0;
  else if (boja.compare("BLUE") == 0)
    boja_vr = 0x001F;
  else if (boja.compare("YELLOW") == 0)
    boja_vr = 0xFFE0;
  else if (boja.compare("BLACK") == 0)
    boja_vr = 0x0000;
  else if (boja.compare("WHITE") == 0)
    boja_vr = 0xFFFF;

  return boja_vr;
}

void background_write(int boja_vr) {
  FILE *fp;
  for (int y = 0; y < 480; y++) 
  {
    for (int x = 0; x < 640; x++) 
    {
	fp = fopen("/dev/vga_dma", "w");
 	 if (fp == NULL) 
	{
   	 printf("Cannot open /dev/vga for write\n");
    	exit(EXIT_FAILURE);
 	 }

      fprintf(fp, "%d,%d,%#04x\n", x, y, boja_vr); 
 	fclose(fp);
 	 if (fp == NULL)
	 {
   	 printf("Cannot close /dev/vga\n");
   	 exit(EXIT_FAILURE);
 	 }
    }  
  }
}

void horizontal_write(int x1, int x2, int y, int boja_vr) {
	FILE *fp;
  for (int x=x1 ; x < x2; x++) 
  {
        fp = fopen("/dev/vga_dma", "w");
         if (fp == NULL) 
        {
         printf("Cannot open /dev/vga for write\n");
        exit(EXIT_FAILURE);
         }

      fprintf(fp, "%d,%d,%#04x\n", x, y, boja_vr); 
        fclose(fp);
         if (fp == NULL)
         {
         printf("Cannot close /dev/vga\n");
         exit(EXIT_FAILURE);
         }  
  }
}
  

void vertical_write(int x, int y1, int y2, int boja_vr) {
  FILE *fp;
  for (int y=y1 ; y < y2; y++) 
  {
        fp = fopen("/dev/vga_dma", "w");
         if (fp == NULL) 
        {
         printf("Cannot open /dev/vga for write\n");
        exit(EXIT_FAILURE);
         }

      fprintf(fp, "%d,%d,%#04x\n", x, y, boja_vr); 
        fclose(fp);
         if (fp == NULL)
         {
         printf("Cannot close /dev/vga\n");
         exit(EXIT_FAILURE);
         }  
  }
}

void rect_write(int x1, int x2, int y1, int y2, int boja_vr) {
   FILE *fp;
  for (int y = y1; y < y2; y++) 
  {
    for (int x = x1; x < x2; x++) 
    {
        fp = fopen("/dev/vga_dma", "w");
         if (fp == NULL) 
        {
         printf("Cannot open /dev/vga for write\n");
        exit(EXIT_FAILURE);
         }

      fprintf(fp, "%d,%d,%#04x\n", x, y, boja_vr); 
        fclose(fp);
         if (fp == NULL)
         {
         printf("Cannot close /dev/vga\n");
         exit(EXIT_FAILURE);
         }
    }  
  }
}
 
int main(int argc, char **argv) {
  int x1, x, x2, y1, y2, y;
  int boja_vr;
  std::string line;
  std::fstream infile;
  if (argc < 2) {
    std::cout << "Please enter config file path\n";
    return -1;
  } else {
    infile.open(argv[1], std::ifstream::in);
    if (infile.fail()) {
      std::cout << "Failed to read file: " << '"' << argv[1] << '"'
                << std::endl;
      return -1;
    }
  }

  while (getline(infile, line)) {

    regex line_reg("(BCKG|LINE_H|LINE_V|RECT):\\s*");
    smatch m;
    std::regex_search(line, m, line_reg);
    cout << m[1] << endl;
    if (m[1].compare("BCKG") == 0) {
      regex bckg("\\b(\\w+)\\s*$");
      smatch b;
      std::regex_search(line, b, bckg);
      boja_vr = Boja(b[1]);
      background_write(boja_vr);
    }

    if (m[1].compare("LINE_H") == 0) {
      regex line_h("(\\d+),\\s*(\\d+),\\s*(\\d+);\\s*(\\w+)\\s*$");
      smatch lh;
      std::regex_search(line, lh, line_h);
      x1 = std::stoi(lh[1]);
      x2 = std::stoi(lh[2]);
      y = std::stoi(lh[3]);
      boja_vr = Boja(lh[4]);
      horizontal_write(x1, x2, y, boja_vr);
    }

    if (m[1].compare("LINE_V") == 0) {
      regex line_v("(\\d+),\\s*(\\d+),\\s*(\\d+);\\s*(\\w+)\\s*$");
      smatch lv;
      std::regex_search(line, lv, line_v);
      x = std::stoi(lv[1]);
      y1 = std::stoi(lv[2]);
      y2 = std::stoi(lv[3]);
      boja_vr = Boja(lv[4]);
      vertical_write(x, y1, y2, boja_vr);
    }

    if (m[1].compare("RECT") == 0) {
      regex rect("(\\d+),\\s*(\\d+),\\s*(\\d+),\\s*(\\d+);\\s*(\\w+)\\s*$");
      smatch r;
      std::regex_search(line, r, rect);
      x1 = std::stoi(r[1]);
      x2 = std::stoi(r[2]);
      y1 = std::stoi(r[3]);
      y2 = std::stoi(r[4]);
      boja_vr = Boja(r[5]);
      rect_write(x1, x2, y1, y2, boja_vr);
    }
  }
  infile.close();
  return 0;
}
