// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de

/* Backpropagation Feedforward Net */
#include <stdio.h>	/* printf(),NULL */
#include <stdlib.h> /* rand() srand() */
#include <math.h>	/* exp() (use: gcc backprop.xx -lm) */
#include "myBackpropNet.h"
#define  BIAS 1.0
#define  ACTFUNC 1 

myBackpropNet::myBackpropNet(int Lay,int *Neu,double gam,double spread,double momentum)
{
	int x;
	m_Lay		= Lay;
	m_Spread	= spread;
	m_Gam		= gam;	
	m_Momentum	= momentum;
	m_Seedrand	= 0.1;
	m_iActFunc	= ACTFUNC;
	m_dBias		= BIAS;

	m_Neur = new int[m_Lay];
	for(x=0;x<m_Lay;x++)        
	{
		m_Neur[x] = Neu[x];
	}
	
	Create();
	Init();
}

myBackpropNet::myBackpropNet(const char* FileName)
{
	int x,y,z;
	FILE* rdFile;

	m_iActFunc	= ACTFUNC;
	m_dBias		= BIAS;

	rdFile = fopen(FileName,"rb");

//	fread(&m_Lay,sizeof(int),1,rdFile);
	fscanf(rdFile,"%d\n",&m_Lay);

	m_Neur = new int[m_Lay];
	for(x=0;x<m_Lay;x++)        
	{
//		fread(&m_Neur[x],sizeof(int),1,rdFile);
		fscanf(rdFile,"%d\n",&m_Neur[x]);
	}


//	fread(&m_Gam,sizeof(double),1,rdFile);
//	fread(&m_Spread,sizeof(double),1,rdFile);
//	fread(&m_Momentum,sizeof(double),1,rdFile);

	fscanf(rdFile,"%lf\n",&m_Gam);
	fscanf(rdFile,"%lf\n",&m_Spread);
	fscanf(rdFile,"%lf\n",&m_Momentum);

	Create();

	for(x=1;x<m_Lay;x++)
	{
		for(y=0;y<m_Neur[x];y++)
		{
			for(z=0;z<m_Neur[x-1];z++)
			{
//				fread(&m_Weig[x][y][z],sizeof(double),1,rdFile);
//				fread(&m_Tmp2[x][y][z],sizeof(double),1,rdFile);

				fscanf(rdFile,"%lf\n",&m_Weig[x][y][z]);
				fscanf(rdFile,"%lf\n",&m_Tmp2[x][y][z]);
			}
//			fread(&m_Weig[x][y][z],sizeof(double),1,rdFile);
//			fread(&m_Tmp2[x][y][z],sizeof(double),1,rdFile);

			fscanf(rdFile,"%lf\n",&m_Weig[x][y][z]);
			fscanf(rdFile,"%lf\n",&m_Tmp2[x][y][z]);
		}
	}

	fclose(rdFile);
}

myBackpropNet::~myBackpropNet()
{
	int x,y;
	
	for(x=1;x<m_Lay;x++)
	{
		for(y=0;y<m_Neur[x];y++)
		{
			delete m_Weig[x][y];
			delete m_Tmp1[x][y];
			delete m_Tmp2[x][y];
		}
		delete m_Delt[x];
		delete m_Weig[x];
		delete m_Tmp1[x];
		delete m_Tmp2[x];
	}
	delete m_Delt;
	delete m_Weig;
	delete m_Tmp1;
	delete m_Tmp2;
	for(x=0;x<m_Lay;x++)
	{
		delete m_Out[x];
	}
	delete m_Out;
	delete m_Gain;
}

int myBackpropNet::Create()
{
	int x,y;

	m_Gain = new double[m_Neur[m_Lay - 1]];
	m_Out  = new double*[m_Lay];
	
	for(x=0;x<m_Lay;x++)
	{
		m_Out[x] = new double[m_Neur[x]];
	}

	m_Weig = new double**[m_Lay];
	m_Tmp1 = new double**[m_Lay];
	m_Tmp2 = new double**[m_Lay];
	m_Delt = new double*[m_Lay];
	m_Weig[0] = NULL;
	m_Tmp1[0] = NULL;
	m_Tmp2[0] = NULL;
	m_Delt[0] = NULL;	

	for(x=1;x<m_Lay;x++)
	{
		m_Weig[x] = new double*[m_Neur[x]];
		m_Tmp1[x] = new double*[m_Neur[x]];
		m_Tmp2[x] = new double*[m_Neur[x]];
		m_Delt[x] = new double[m_Neur[x]];
		
		for(y=0;y<m_Neur[x];y++)
		{
			// eins mehr wegen dem Bias
			m_Weig[x][y] = new double[m_Neur[x-1] + 1];
			m_Tmp1[x][y] = new double[m_Neur[x-1] + 1];
			m_Tmp2[x][y] = new double[m_Neur[x-1] + 1];
		}
	}

	return 1;
}

void myBackpropNet::Init()
{
	int x,y,z;
	double h;

	for(x=1;x<m_Lay;x++)
	{
		for(y=0;y<m_Neur[x];y++)
		{
			for(z=0;z<=m_Neur[x-1];z++)
			{
				/* Random weights */
				h = m_Seedrand * (1.0 - (2.0 * rand())/RAND_MAX);
				m_Weig[x][y][z] = h;
				m_Tmp2[x][y][z] = h;
			}
		}
	}
}

void myBackpropNet::Input(double *In)
{
	int x;
	
	for(x=0;x<m_Neur[0];x++)
	{
		m_Out[0][x] = In[x];
	}
}

void myBackpropNet::Desired(double *Des)
{
	int x;

	for(x=0;x<m_Neur[m_Lay-1];x++)
	{
		m_Gain[x] = Des[x];
	}
}

void myBackpropNet::Output(double *Out)
{
	int x;
	
	for(x=0;x<m_Neur[m_Lay-1];x++)
	{
		Out[x] = m_Out[m_Lay-1][x];
	}
}

double myBackpropNet::ActFunc(double dInp)
{
	if(m_iActFunc == 1)
		return 1.0 / (1.0 + exp(-dInp * m_Spread));
	if(m_iActFunc == 2)
		return 1.0 / (1.0 + exp(-dInp * m_Spread)) - 0.5;
	return 1.0;
}

double myBackpropNet::InvActFunc(double dInp)
{
	return dInp * (1 - dInp);
}

void myBackpropNet::Calc()
{
	int x,y,z;
	double h;
	for(x=1;x<m_Lay;x++)
	{
		for(y=0;y<m_Neur[x];y++)
		{
			h = -0.0f;
			for(z=0;z<m_Neur[x-1];z++)
            {
				h += m_Out[x-1][z] * m_Weig[x][y][z];
			}
 
        	h += m_dBias * m_Weig[x][y][z];  
			m_Out[x][y] = ActFunc(h);
		}
	}
}

void myBackpropNet::Learn()
{
	int x,y,z;
	double h;

	/* Backpropagation Fehler in der Ausgabeschich*/
	for(x=0;x<m_Neur[m_Lay-1];x++)
	{  
		// Fehler in der Ausgabeschicht
		m_Delt[m_Lay-1][x] = (m_Gain[x] - m_Out[m_Lay-1][x]) * InvActFunc(m_Out[m_Lay-1][x]);
	}

	/* Backpropagation Fehler in den restlichen Schichten*/
	for(x=m_Lay-1;x>1;x--)
	{
		for(y=0;y<m_Neur[x-1];y++)
		{
			h = 0.0;
			for(z=0;z<m_Neur[x];z++)
			{
				h += m_Delt[x][z] * m_Weig[x][z][y];
			}
			m_Delt[x-1][y] = InvActFunc(m_Out[x-1][y]) * h;
		}
	}
	
	/* errorcorrection */
	for(x=1;x<m_Lay;x++)
	{
		for(y=0;y<m_Neur[x];y++)
		{
			for(z=0;z<m_Neur[x-1];z++)
			{
				h = m_Out[x-1][z] * m_Gam * m_Delt[x][y];
				h += m_Momentum * (m_Weig[x][y][z] - m_Tmp2[x][y][z]);
				m_Tmp1[x][y][z] = m_Weig[x][y][z] + h; 
			}
			h = m_dBias * m_Gam * m_Delt[x][y];
			h += m_Momentum * (m_Weig[x][y][z] - m_Tmp2[x][y][z]);
			m_Tmp1[x][y][z] = m_Weig[x][y][z] + h;
		}
	}

	/* kopiere W -> t2 und t1 -> W */
	for(x=1;x<m_Lay;x++)
	{
		for(y=0;y<m_Neur[x];y++)
		{
			for(z=0;z<m_Neur[x-1];z++)
			{
				m_Tmp2[x][y][z] = m_Weig[x][y][z];
				m_Weig[x][y][z] = m_Tmp1[x][y][z];
			}
			m_Tmp2[x][y][z] = m_Weig[x][y][z];
			m_Weig[x][y][z] = m_Tmp1[x][y][z];
		}
	}
}

int myBackpropNet::Save(const char* FileName)
{
	int x,y,z;
	FILE* wrFile;

	wrFile = fopen(FileName,"wb");
	if(wrFile==NULL)
	{
		return 0;
	}

//	fwrite(&m_Lay,sizeof(int),1,wrFile);
	fprintf(wrFile,"%d\n",m_Lay);

	for(x=0;x<m_Lay;x++)
	{
	//	fwrite(&m_Neur[x],sizeof(int),1,wrFile);
		fprintf(wrFile,"%d\n",m_Neur[x]);
	}

//	fwrite(&m_Gam,sizeof(double),1,wrFile);
//	fwrite(&m_Spread,sizeof(double),1,wrFile);
//	fwrite(&m_Momentum,sizeof(double),1,wrFile);

	fprintf(wrFile,"%f\n",m_Gam);
	fprintf(wrFile,"%f\n",m_Spread);
	fprintf(wrFile,"%f\n",m_Momentum);

	for(x=1;x<m_Lay;x++)
	{
		for(y=0;y<m_Neur[x];y++)
		{
			for(z=0;z<m_Neur[x-1];z++)
			{
//				fwrite(&m_Weig[x][y][z],sizeof(double),1,wrFile);
//				fwrite(&m_Tmp2[x][y][z],sizeof(double),1,wrFile);
				
				fprintf(wrFile,"%f\n",m_Weig[x][y][z]);
				fprintf(wrFile,"%f\n",m_Tmp2[x][y][z]);
			}
//			fwrite(&m_Weig[x][y][z],sizeof(double),1,wrFile);
//			fwrite(&m_Tmp2[x][y][z],sizeof(double),1,wrFile);

			fprintf(wrFile,"%f\n",m_Weig[x][y][z]);
			fprintf(wrFile,"%f\n",m_Tmp2[x][y][z]);
		}
	}

	fclose(wrFile);

	return 1;
}

