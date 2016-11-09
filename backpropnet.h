// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de

#ifndef BACKPROPNET
#define BACKPROPNET

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

template <class T, int MAX>class FixArray 
{
protected:
    T array[MAX];

    int index_check(int i) const 
	{
       if ((i<0)||(i>=MAX)) 
	   {
		   return 0;
       }
	   return 1;
    }

public:
    int get_max() const { return MAX; };

    T operator[](int i) const 
	{
       index_check(i);
       return array[i];
    };

    T& operator[](int i) 
	{
       index_check(i);
       return array[i];
    };
};

template <int c0, int c1, int c2>
class BackpropNet 
{
     double total_error;
     double max_error;
     double pat_max_error;

     FixArray<double,c2> e2;
     FixArray<FixArray<double,c1>, c0+1> n01;
     FixArray<FixArray<double,c2>, c1+1> n12;

     FixArray<FixArray<double,c1>, c0+1> g01;
     FixArray<FixArray<double,c2>, c1+1> g12;

     FixArray<double,c0+1> o0;
     FixArray<double,c1+1> o1;
     FixArray<double,c2> o2;
     FixArray<double,c2> d;

     FixArray<FixArray<double,c1>, c0+1> w01;
     FixArray<FixArray<double,c2>, c1+1> w12;

     double sigmoid(double p, double spread) { return 1.0/(1.0+exp(-spread*p)); };
     double inv_sigmoid(double o, double spread) { return spread*o*(1.0-o); };

     void calc_error() 
     {
         pat_max_error=0;
         for (int t=0; t<o2.get_max(); t++) 
	 {
             if (fabs(d[t]-o2[t])>pat_max_error) pat_max_error=fabs(d[t]-o2[t]);
             total_error+=(o2[t]-d[t])*(o2[t]-d[t]);
         }
         if (pat_max_error>max_error) max_error=pat_max_error;
     };

public:
     double gain;
     double spread;
     double momentum;
     double seedrand;

     BackpropNet()
	 {
		 gain = 0.1;
		 spread = 1.0;
		 momentum = 0.1;
		 seedrand = 0.1;
         reset();
         o0[o0.get_max()-1]=1.0;
         o1[o1.get_max()-1]=1.0;
     };

     void reset() 
	 {
		 int i;
		 int j;
         for(i=0; i<o0.get_max(); i++)
             for(j=0; j<o1.get_max()-1; j++)
                 w01[i][j]=seedrand*(1.0-(2.0*rand())/RAND_MAX);
         for(i=0; i<o1.get_max(); i++)
             for(j=0; j<o2.get_max(); j++)
                 w12[i][j]=seedrand*(1.0-(2.0*rand())/RAND_MAX);
         g01=w01;
         g12=w12;
     };

     double &input(int i) { return o0[i]; };
     double output(int i) { return o2[i]; };
     double &desired(int i) { return d[i]; };

     void feedforward() 
	 {
          double z;
		  int j;
          for(j=0; j<o1.get_max()-1; j++) 
		  {
              z=0;
              for (int i=0; i<o0.get_max(); i++)
                  z+=o0[i]*w01[i][j];
              o1[j]=sigmoid(z, spread);
          }

          for(j=0; j<o2.get_max(); j++) 
		  {
              z=0;
              for (int i=0; i<o1.get_max(); i++)
                  z+=o1[i]*w12[i][j];
              o2[j]=sigmoid(z, spread);
          }

          calc_error();
     };

     void backpropagate()
	 {
          double tmp, e1;
		  int j;
		  int k;
		  int i;
          for(j=0; j<o2.get_max(); j++) 
		  {
              e2[j]=inv_sigmoid(o2[j], spread) * (d[j]-o2[j]);
              for(i=0; i<o1.get_max(); i++)
                  n12[i][j]=w12[i][j] + gain*e2[j]*o1[i] + momentum*(w12[i][j]-g12[i][j]);
          }
          for(j=0; j<o1.get_max()-1; j++) 
		  {
              tmp=0;
              for(k=0; k<o2.get_max(); k++)
                  tmp+=e2[k]*w12[j][k];
              e1=inv_sigmoid(o1[j], spread) * tmp;
              for(i=0; i<o0.get_max(); i++)
                  n01[i][j]=w01[i][j] + gain*e1*o0[i] + momentum*(w01[i][j]-g01[i][j]);
          }
          g01=w01;
          g12=w12;
          w01=n01;
          w12=n12;
     };

     void reset_max_error() { max_error=0.0; };
     double get_max_error() { return max_error; };
     void reset_total_error() { total_error=0.0; };
     double get_total_error() { return total_error/2; };

     int load(const char *filename) 
     {
         FILE *fh;
         int cv0 = c0, cv1 = c1, cv2 = c2;

         if ((fh=fopen(filename, "rb"))==NULL) 
		 {
			 return 0;
		 };
	 
         fread(&gain,sizeof(double),1,fh);
         fread(&spread,sizeof(double),1,fh);
	 fread(&momentum,sizeof(double),1,fh);
	 fread(&seedrand,sizeof(double),1,fh);
         fread(&cv0, sizeof(int), 1, fh);
         fread(&cv1, sizeof(int), 1, fh);
         fread(&cv2, sizeof(int), 1, fh);

         if ((cv0!=o0.get_max()-1)||
             (cv1!=o1.get_max()-1)||
             (cv2!=o2.get_max())) 
		 {
		   return 0;
		 }

         for (int i=0; i<c0+1; i++)
             for (int j=0; j<c1; j++)
                 fread(&w01[i][j], sizeof(double), 1, fh);

         for (int j=0; j<c1+1; j++)
             for (int k=0; k<c2; k++)
                 fread(&w12[j][k], sizeof(double), 1, fh);

         fclose(fh);
		 return 1;
     };

     int save(const char *filename) 
     {
         FILE *fh;
	 int cv0, cv1, cv2;

         if ((fh=fopen(filename, "wb"))==NULL) 
		 {
			 return 0;
         };

         cv0=o0.get_max()-1;
         cv1=o1.get_max()-1;
         cv2=o2.get_max();

	 fwrite(&gain,sizeof(double),1,fh);
	 fwrite(&spread,sizeof(double),1,fh);
	 fwrite(&momentum,sizeof(double),1,fh);
	 fwrite(&seedrand,sizeof(double),1,fh);
         fwrite(&cv0, sizeof(int), 1, fh);
         fwrite(&cv1, sizeof(int), 1, fh);
         fwrite(&cv2, sizeof(int), 1, fh);

         for (int i=0; i<c0+1; i++)
             for (int j=0; j<c1; j++)
                 fwrite(&w01[i][j], sizeof(double), 1, fh);

         for (int j=0; j<c1+1; j++)
             for (int k=0; k<c2; k++)
                 fwrite(&w12[j][k], sizeof(double), 1, fh);

         fclose(fh);
		 return 1;
     };
};

#endif
