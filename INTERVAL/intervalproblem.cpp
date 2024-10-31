#include "intervalproblem.h"
# include <QVariant>

IntervalProblem::IntervalProblem(int d)
{
    setDimension(d);
    modelSeed=1;
    generator.seed(modelSeed);
}

void    IntervalProblem::setDimension(int d)
{
    dimension=d;
    margin.resize((unsigned)d);
    for(unsigned int i=0;i<d;i++)
        margin[i]=Interval(-100,100);
}

void    IntervalProblem::setParameter(QString name,QVariant value)
{
    //nothing here
}

void   IntervalProblem::setMargins(IntervalData &m)
{
    margin=m;
}

IntervalData IntervalProblem::getMargins() const
{
    return margin;
}

int    IntervalProblem::getDimension() const
{
    return dimension;
}

double  IntervalProblem::randomDouble()
{
    return doubleDistrib(generator);
}

bool   IntervalProblem::isPointIn(Data &x) const
{
    for(int i=0;i<margin.size();i++)
    {
        if(x[i]<margin[i].leftValue() || x[i]>margin[i].rightValue())
            return false;
    }
    return true;
}

/*
Interval IntervalProblem::IntFunmin(IntervalData &x)
{
    return ifunmin(x);
}*/

double  IntervalProblem::getVolume(IntervalData &x) const
{
    double s=0;
    for(unsigned i=0;i<x.size();i++)
        s+=x[i].width();
    return s;
}

double  IntervalProblem::getVolume() const
{
    return 0.0;
}

void    IntervalProblem::printData(IntervalData &x)
{
    double totalWidth=0.0;
    for(int i=0;i<x.size();i++)
    {
        cout<<">>"<<x[i].toString().toStdString()<<",";
        totalWidth+=(x[i].rightValue()-x[i].leftValue());
    }
    cout<<" Width = "<<totalWidth<<  endl;
}

IntervalData    IntervalProblem::createRandomInterval()
{
    IntervalData x;
    x.resize(dimension);
    for(int i=0;i<dimension;i++)
        x[i]=margin[i];
    for(int i=0;i<dimension;i++)
        randomize(x,i);
    /*
    for(int i=0;i<dimension;i++)
    {
        x[i]=margin[i];
    }
    for(int i=0;i<dimension;i++)
    {
        int tries=5;
        for(int j=0;j<tries;j++)
        {
            int l=rand()%2;
            int r=rand()%2;
            double origLeft=x[i].left();
            if(l==1) origLeft/=2;
            if(origLeft<margin[i].left()) origLeft=margin[i].left();
            x[i]=interval(origLeft,x[i].right());
            double origRight=x[i].right();
            if(r==1)
            {origRight/=2;
                if(origRight<margin[i].left())
                    origRight=x[i].right();
                if(origRight<origLeft)
                {

                x[i]=interval(origRight,origLeft);
                }
                else x[i]=interval(origLeft,origRight);

            }
        }
    }*/
    return x;
}

void    IntervalProblem::randomize(IntervalData &x,int pos)
{
   double r1=randomDouble();
   double r2=randomDouble();
   double lvalue=x[pos].leftValue();
   double rvalue=x[pos].rightValue();
   //x[pos]=lvalue+(rvalue-lvalue)*r1;
	x[pos]=Interval(r1*lvalue,r2*rvalue);
}


bool   IntervalProblem::lowerValue(Interval &a,Interval &b)
{

     return a.leftValue()<b.leftValue()
       || (a.leftValue()<=b.leftValue() &&
           fabs(a.leftValue()-b.leftValue())<1e-6
           && a.width()<b.width());
}

bool    IntervalProblem::isPointIn(IntervalData &x) const
{
    for(int i=0;i<x.size();i++)
    {
        if(x[i].leftValue()<margin[i].leftValue()
         || x[i].rightValue()>margin[i].rightValue()) return false;
    }
    return true;
}
void    IntervalProblem::boundInterval(IntervalData &x)
{
    for(int i=0;i<x.size();i++)
    {
       double d1=(x[i].leftValue()<margin[i].leftValue())?margin[i].leftValue():x[i].leftValue();
       double d2=(x[i].rightValue()>margin[i].rightValue())?margin[i].rightValue():x[i].rightValue();
        d1=x[i].leftValue();
        d2=x[i].rightValue();
       if(d1>margin[i].rightValue()) d1=margin[i].rightValue();
        if(d1<margin[i].leftValue()) d1=margin[i].leftValue();
        if(d2<margin[i].leftValue()) d2=margin[i].leftValue();
        if(d2>margin[i].rightValue()) d2=margin[i].rightValue();

       if(d1>d2)
       {
           double t=d1;
           d1=d2;
           d2=t;
       }

       x[i]=Interval(d1,d2);
    }
}

Data    IntervalProblem::getSample()
{
    Data xx;
    xx.resize(getDimension());
    for(int i=0;i<(int)xx.size();i++)
        xx[i]=margin[i].leftValue()+randomDouble()*(margin[i].rightValue()-margin[i].leftValue());
    return xx;
}

void    IntervalProblem::init(QJsonObject x)
{
    //nothing here
}

double  IntervalProblem::funmin(Data &x)
{
    return 0.0;
}

void    IntervalProblem::granal(Data &x, Data &g)
{
    //nothing here
}

QJsonObject IntervalProblem::done(Data &x)
{
    return QJsonObject();
}

void    IntervalProblem::setModelSeed(int seed)
{
    modelSeed = seed;
    generator.seed(modelSeed);

}

double  IntervalProblem::grms(Data &x)
{
    Data g;
    g.resize(x.size());
    granal(x,g);
    double sum =0.0;
    for(int i=0;i<(int)x.size();i++)
        sum+=g[i]*g[i];
    return sum;
}

bool     IntervalProblem::getGradientCriterion(Data &x1,Data &x2)
{
    Data g1,g2;
    g1.resize(x1.size());
    g2.resize(x2.size());
    granal(x1,g1);
    granal(x2,g2);
    double s=0.0;
    for(int i=0;i<x1.size();i++)
        s+=(x1[i]-x2[i])*(g1[i]-g2[i]);
    return s>=0;
}

int     IntervalProblem::getModelSeed() const
{
    return modelSeed;
}

IntervalProblem::~IntervalProblem()
{
//nothing here
}
