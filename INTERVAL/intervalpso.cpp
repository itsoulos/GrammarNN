#include "intervalpso.h"
# include <stdlib.h>
IntervalPso::IntervalPso(IntervalProblem *p)
{
    problem = p;
    plist.addParam(Parameter("pso_particles",500,10,1000,"Number of particles"));
    plist.addParam(Parameter("pso_iterations",200,10,2000,"Number of iterations"));
    plist.addParam(Parameter("pso_c1",0.1,0.0,2.0,"Pso c1 parameter"));
    plist.addParam(Parameter("pso_c2",0.1,0.0,2.0,"Pso c2 parameter"));
    plist.addParam(Parameter("pso_wmin",0.4,0.0,1.0,"Pso wmin parameter"));
    plist.addParam(Parameter("pso_wmax",0.9,0.0,1.0,"Pso wmax parameter"));
}
void        IntervalPso::setProblem(IntervalProblem *p)
{
    problem = p;
}

void        IntervalPso::setParam(QString name,QString value,QString help)
{
    plist.setParam(name,value,help);
}

Interval    IntervalPso::fitness(IntervalData &x)
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

void        IntervalPso::Solve()
{
    //init
    int pso_particles = plist.getParam("pso_particles").getValue().toInt();
    int pso_iterations = plist.getParam("pso_iterations").getValue().toInt();
    double pso_c1 = plist.getParam("pso_c1").getValue().toDouble();
    double pso_c2 = plist.getParam("pso_c2").getValue().toDouble();
    double pso_wmin = plist.getParam("pso_wmin").getValue().toDouble();
    double pso_wmax = plist.getParam("pso_wmax").getValue().toDouble();
    particle.resize(pso_particles);
    bestParticle.resize(pso_particles);
    velocity.resize(pso_particles);
    fitnessArray.resize(pso_particles);
    bestFitnessArray.resize(pso_particles);
    IntervalData m  = problem->getMargins();
    int n = problem->getDimension();
    bestx.resize(n);
    for(int i=0;i<pso_particles;i++)
    {

        particle[i].resize(n);
        bestParticle[i].resize(n);
        velocity[i].resize(n);
        for(int j=0;j<n;j++)
        {
            double left =m[j].leftValue();
            double right = m[j].rightValue();
            double mid = left+(right-left)/2.0;
            double l = left+problem->randomDouble()*(mid-left);
            double r = mid+problem->randomDouble()*(right-mid);
            particle[i][j]=Interval(l,r);
            bestParticle[i][j]=particle[i][j];
        }
    }
    for(int i=0;i<pso_particles;i++)
    {
        fitnessArray[i]=fitness(particle[i]);
        bestFitnessArray[i]=fitnessArray[i];
    }
    getBest(bestx,besty);
    //init velocity
    for(int i=0;i<pso_particles;i++)
    {
        for(int j=0;j<n;j++)
        {
            double left =m[j].leftValue();
            double right = m[j].rightValue();
            double width = right -left;
            double a =  - width/10;
            double b =  width/10;
            velocity[i][j]=Interval(a,b);
        }
    }
    //run
    for(int k=0;k<pso_iterations;k++)
    {
        //calc inertia
        inertia = pso_wmin +(pso_wmax-pso_wmin)*(k+1.0)/pso_iterations;
        inertia = 0.5 + drand48()/2.0;
        //calc velocity
        for(int i=0;i<pso_particles;i++)
        {
            for(int j=0;j<n;j++)
            {
                double r1 = drand48();
                double r2 = drand48();

                double left=0,right=0;
                left =r1 * pso_c1 *
                         (bestParticle[i][j]-particle[i][j]).width()+
                        r2 * pso_c2 * (bestx[j]-particle[i][j]).width();
                right =r1 * pso_c1 *
                         (bestParticle[i][j]-particle[i][j]).width()+
                        r2 * pso_c2 * (bestx[j]-particle[i][j]).width();

                Interval oldVel = velocity[i][j];
                velocity[i][j]=Interval(inertia * velocity[i][j].leftValue()+0.5*left,
                                        inertia * velocity[i][j].rightValue()-
                                        0.5*right);
                //update positions
                Interval oldData = particle[i][j];
                particle[i][j]=particle[i][j]+velocity[i][j];

                if(particle[i][j].leftValue()<m[j].leftValue() ||
                        particle[i][j].rightValue()>m[j].rightValue())
                {
                   /* printf("Out of bounds [%lf,%lf] from [%lf,%lf]\n",
                           particle[i][j].leftValue(),
                           particle[i][j].rightValue(),
                           m[j].leftValue(),m[j].rightValue());*/
                    particle[i][j]=oldData;
                   velocity[i][j]=oldVel;
                }
            }
        //update fitness
            fitnessArray[i]=fitness(particle[i]);

            if(problem->lowerValue(fitnessArray[i],bestFitnessArray[i]))
            {
                bestParticle[i]=particle[i];
                bestFitnessArray[i]=fitnessArray[i];
                if(problem->lowerValue(fitnessArray[i],besty))
                {
                    besty = fitnessArray[i];
                    bestx = particle[i];
                }
            }
        }
        printf("PSO. Iteration=%4d Best Value=[%10.5lf,%10.5lf]\n",
               k,besty.leftValue(),besty.rightValue());
    }
}

void        IntervalPso::getBest(IntervalData &x,Interval &y)
{
    y = fitnessArray[0];
    x = particle[0];
    for(int i=0;i<(int)fitnessArray.size();i++)
    {
        if(problem->lowerValue(fitnessArray[i],y))
        {
            y = fitnessArray[i];
            x = particle[i];
        }
    }
}

ParameterList IntervalPso::getParameterList() const
{
    return plist;
}

IntervalPso::~IntervalPso()
{
    //nothing here
}
