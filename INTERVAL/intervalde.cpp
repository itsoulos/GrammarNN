#include "intervalde.h"

IntervalDE::IntervalDE(IntervalProblem *p)
{
    problem = p;
    plist.addParam(Parameter("ide_np",200,10,500,"Number of agents"));
    plist.addParam(Parameter("ide_f",0.8,0.0,2.0,"Differential Weight"));
    plist.addParam(Parameter("ide_cr",0.9,0.0,1.0,"Crossover rate"));
    plist.addParam(Parameter("ide_maxiters",500,10,2000,"Number of iterations"));
}


void    IntervalDE::setProblem(IntervalProblem *p)
{
    problem = p;
}

void        IntervalDE::setParam(QString name,QString value,QString help)
{
    plist.setParam(name,value,help);
}

Interval    IntervalDE::fitness(IntervalData &x)
{

    vector<Interval> oldMargin = problem->getMargins();
    vector<Interval> newMargin = x;

    double miny=1e+100,maxy=1e+100;
    Data trialx;
    trialx.resize(problem->getDimension());

    problem->setModelSeed(1);
    problem->setMargins(newMargin);
    for(int k=1;k<=nsamples;k++)
    {

        trialx=problem->getSample();
        double fx= problem->funmin(trialx);
        if(k==1 || fx>maxy) maxy=fx;
        if(k==1 || fx<miny) miny=fx;
    }
    problem->setMargins(oldMargin);
    return Interval(miny,maxy);
}

void        IntervalDE::Solve()
{
    NP = plist.getParam("ide_np").getValue().toInt();
    F  = plist.getParam("ide_f").getValue().toDouble();
    CR = plist.getParam("ide_cr").getValue().toDouble();
    const int iters = plist.getParam("ide_maxiters").getValue().toInt();
    agent.resize(NP);
    fitnessArray.resize(NP);

    //init
    IntervalData m  = problem->getMargins();
    for(int i=0;i<NP;i++)
    {

        int n = problem->getDimension();
        agent[i].resize(n);
        for(int j=0;j<n;j++)
        {
            double left =m[j].leftValue();
            double right = m[j].rightValue();
            double mid = left+(right-left)/2.0;
            double l = left+problem->randomDouble()*(mid-left);
            double r = mid+problem->randomDouble()*(right-mid);
            agent[i][j]=Interval(l,r);
        }
        fitnessArray[i]=fitness(agent[i]);
    }
    //run
    for(int iter=1;iter<=iters;iter++)
    {
        for(int i=0;i<NP;i++)
        {
            int a,b,c;
            do
            {
                a = rand() % NP;
                b = rand() % NP;
                c = rand() % NP;
            }while(a==b || b==c || c==a
                   || a==i || b==i || c==i
                   );
            IntervalData xa = agent[a];
            IntervalData xb = agent[b];
            IntervalData xc = agent[c];

            int n = problem->getDimension();
            int index = rand() % n;
            IntervalData trialx = agent[i];
            for(int j=0;j<n;j++)
            {
                if(j==index || problem->randomDouble()<=CR)
                {
                    double left,right;
                    left = xa[j].leftValue()+F*(xb[j].leftValue()-xc[j].leftValue());
                    right= xa[j].rightValue()+F*(xb[j].rightValue()-xc[j].rightValue());
                    trialx[j]=Interval(left,right);
                }
            }
            Interval trialf = fitness(trialx);
            if(problem->lowerValue(trialf,fitnessArray[i]))
            {
                agent[i]=trialx;
                fitnessArray[i]=trialf;

            }

        }
        Interval y=fitnessArray[0];
        for(int i=0;i<(int)fitnessArray.size();i++)
        {
            if(problem->lowerValue(fitnessArray[i],y))
            {
                y=fitnessArray[i];
            }
        }
        printf("BEST[%d]=[%lf %lf]\n",iter,y.leftValue(),y.rightValue());
    }
}

void        IntervalDE::getBest(IntervalData &x,Interval &y)
{
    y=fitnessArray[0];
    x = agent[0];
    for(int i=0;i<(int)fitnessArray.size();i++)
    {
        if(problem->lowerValue(fitnessArray[i],y))
        {
            y=fitnessArray[i];
            x = agent[i];
        }
    }
}

IntervalDE::~IntervalDE()
{
    //nothing here
}
