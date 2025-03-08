#include "intervalanneal.h"

IntervalAnneal::IntervalAnneal(IntervalProblem *p)
{
    problem = p;
    neps = 50;
    alpha = 0.9;
    T0=1e+8;
    plist.addParam(Parameter("isiman_t0",1e+8,1e+3,1e+10,"Initial temp"));
    plist.addParam(Parameter("isiman_neps",10,10,200,"Number of random points"));
    plist.addParam(Parameter("isiman_alpha",0.995,0.1,1.0,"Alpha value for temp"));
}

void            IntervalAnneal::setProblem(IntervalProblem *p)
{
    problem = p;
}

void            IntervalAnneal::setParam(QString name,QString value,QString help)
{
    plist.addParam(Parameter(name,value,help));
}

Interval        IntervalAnneal::fitness(IntervalData &x)
{
    srand(1);
    Data trialx;
    trialx.resize(x.size());
    double minf=1e+100,maxf=-1e+100;
    for(int i=1;i<=nsamples;i++)
    {
       for(int j=0;j<x.size();j++)
       {
           double left = x[j].leftValue();
           double right =x[j].rightValue();
           trialx[j]=left+(rand()*1.0/RAND_MAX)*(right-left);
       }
       double y=problem->funmin(trialx);
       if(y<minf || i==1) minf = y;
       if(y>maxf || i==1) maxf = y;
    }
    return Interval(minf,maxf);
}

void            IntervalAnneal::Solve()
{
    T0 = plist.getParam("isiman_t0").getValue().toDouble();
    neps = plist.getParam("isiman_neps").getValue().toInt();
    alpha = plist.getParam("isiman_alpha").getValue().toDouble();
    k=1;
    xpoint.resize(problem->getDimension());
    bestx.resize(problem->getDimension());
    xpoint = problem->getMargins();
    bestx = xpoint;
    ypoint = fitness(xpoint);
    besty = ypoint;
    IntervalData trialx = xpoint;
    Interval trialy = ypoint;

    do
    {
    //    xpoint = bestx;
    //    ypoint = besty;
        for(int i=1;i<=neps;i++)
        {
            randomSample(trialx);
            trialy = fitness(trialx);

            if(problem->lowerValue(trialy,ypoint))
            {
                ypoint = trialy;
                xpoint = trialx;
                if(problem->lowerValue(ypoint,besty))
                {
                    besty = ypoint;
                    bestx = xpoint;
                }
            }
            else
            {
                double r = drand48();
                double ratio = exp(-(trialy.leftValue()-ypoint.leftValue())/T0);
                double xmin = ratio<1?ratio:1;
                if(r<xmin)
                {
                    ypoint = trialy;
                    xpoint = trialx;
                }
            }
        }
        reduceTemp();
        printf("TEMP: %20.10lg BESTY=[%20.10lg %20.10lg]\n",
               T0,ypoint.leftValue(),ypoint.rightValue());
    }while(T0>=1e-6);
}

void            IntervalAnneal::getBest(IntervalData &x,Interval &y)
{
    x = bestx;
    y = besty;
}

ParameterList   IntervalAnneal::getParameterList() const
{
    return plist;
}

void            IntervalAnneal::reduceTemp()
{
   T0 =T0 * pow(alpha,k);
   k++;
}

void  IntervalAnneal::randomSample(IntervalData &x)
{

    for(int i=0;i<(int)x.size();i++)
    {
        double left =   xpoint[i].leftValue();
        double right=   xpoint[i].rightValue();
        double mid =    left+(right-left)/2.0;
        double percent = 0.01;
        double a =      left+(mid-left)*drand48()*percent;
        double b =      right-(right-mid)*drand48()*percent;
        x[i]=Interval(a,b);
    }
}

IntervalAnneal::~IntervalAnneal()
{
    //nothing here
}
