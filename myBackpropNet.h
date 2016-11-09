// (C) 2001 by
// // Melanie Sippl melanie.sippl@web.de
// // Ingo Schasiepen ingo.schasiepen@web.de
// // Andre Betz andre.betz@gmx.de

#ifndef MYBACKPROPNET
#define MYBACKPROPNET

class myBackpropNet
{
private:
	int			m_Lay;
	int			m_iActFunc;
	int*		m_Neur;
	double		m_Gam;
	double		m_Spread;
	double		m_Seedrand;
	double		m_Momentum;
	double		m_dBias;
	double*		m_Gain;
	double**	m_Out;
	double**	m_Delt;
	double***	m_Weig;
	double***	m_Tmp1;
	double***	m_Tmp2;
	double ActFunc(double dInp);
	double InvActFunc(double dInp);
	int Create();
	void Init();
public:
	myBackpropNet	(int Lay,int *Neu,double gam,double spread,double momentum);
	myBackpropNet	(const char* FileName);
	~myBackpropNet	();
	void	Input	(double *In);
	void	Desired(double *Des);
	void	Output	(double *Out);
	void	Calc	();
	void	Learn	();
	int	Save(const char* FileName);
};

#endif
