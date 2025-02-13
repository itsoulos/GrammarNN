#include "intervalde.h"

IntervalDE::IntervalDE(IntervalProblem *p)
{
    problem = p;
    plist.addParam(Parameter("ide_np",200,10,500,"Number of agents"));
    plist.addParam(Parameter("ide_f",0.8,0.0,2.0,"Differential Weight"));
    plist.addParam(Parameter("ide_cr",0.9,0.0,1.0,"Crossover rate"));
    QStringList m;
    m<<"random"<<"fixed"<<"adapt"<<"migrant";
    plist.addParam(Parameter("ide_weightmethod",m[0],m,"Used weight method"));
    plist.addParam(Parameter("ide_maxiters",500,10,2000,"Number of iterations"));
}


double  IntervalDE::getAdaptiveWeight(int iter)
{
	int de_maxiters = plist.getParam("ide_maxiters").getValue().toInt();
    const double fmin = 0.2;
    const double fmax = 0.9;
    return fmax - (fmax - fmin )*(iter*1.0/de_maxiters)*(iter*1.0/de_maxiters);
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


    double miny=1e+100,maxy=1e+100;
    Data trialx;
    trialx.resize(problem->getDimension());

    problem->setModelSeed(1);
    for(int k=1;k<=nsamples;k++)
    {

	
        for(int i=0;i<(int)trialx.size();i++)
        {
            trialx[i]=x[i].leftValue()+
                    (x[i].rightValue()-x[i].leftValue())*problem->randomDouble();
        }
        double fx= (problem->funmin(trialx));
        if(k==1 || fx>maxy) maxy=fx;
        if(k==1 || fx<miny) miny=fx;
    }
    return Interval(miny,maxy);
}
void IntervalDE::calculateMigrantWeights()
{
    double maxFitness=fitnessArray[0].leftValue();
    double sumW = 0.0;

    ff.resize(NP);
    vector<double> weights;
    weights.resize(NP);
    for (int i = 0; i < NP; ++i)
    {
        if(fitnessArray[i].leftValue()>maxFitness)
            maxFitness = fitnessArray[i].leftValue();
    }

    for (int i = 0; i < NP; ++i)
    {
        double d = maxFitness - fitnessArray[i].leftValue();
        weights[i] = d;
        sumW += d;
    }


    for (int i = 0; i < NP; ++i)
    {
        ff[i] = weights[i] / sumW;
    }
}

ParameterList IntervalDE::getParameterList() const
{
    return plist;
}

void        IntervalDE::Solve()
{
    NP = plist.getParam("ide_np").getValue().toInt();
    F  = plist.getParam("ide_f").getValue().toDouble();
    CR = plist.getParam("ide_cr").getValue().toDouble();
    QString method = plist.getParam("ide_weightmethod").getValue();
    const int iters = plist.getParam("ide_maxiters").getValue().toInt();
    agent.resize(NP);
    fitnessArray.resize(NP);

    drandDat.resize(10 * nsamples*problem->getDimension());
    for (unsigned i = 0; i < drandDat.size();i++) {
        drandDat[i]=problem->randomDouble();
    }
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
    }
    for(int i=0;i<NP;i++)
    {
        fitnessArray[i]=fitness(agent[i]);
    }
    //run
    for(int iter=1;iter<=iters;iter++)
    {
        if(method == "migrant")
	    calculateMigrantWeights();
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


		    //F = getAdaptiveWeight(iter);
            double f1=F;
            double f2=F;
            if(method == "random")
            {
                f1 = drand48();
                f2 = drand48();
            }
            else
            if(method == "fixed")
            {
                f1 = F;
                f2 = F;
            }
            else
            if(method == "adapt")
            {
                f1 = getAdaptiveWeight(iter);
                f2 = getAdaptiveWeight(iter);
            }
            else
            {
                f1 = ff[i];
                f2 = ff[i];
            }
		    left = xa[j].leftValue() +f1*fabs(xb[j].leftValue()-xc[j].leftValue());
		    right = xa[j].rightValue() -f2*fabs(xb[j].rightValue()-xc[j].rightValue());
                    trialx[j]=Interval(left,right);
		    
		    /*if(problem->randomDouble()<=0.01)
		    {
            			Interval trialf = fitness(trialx);
            			if(problem->lowerValue(trialf,fitnessArray[i]))
            			{
                			agent[i]=trialx;
                			fitnessArray[i]=trialf;
            			}
				else trialx[j]=agent[i][j];
		    }*/
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
	if(iter%50==0)
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
