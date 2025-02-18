#ifndef INTERVALPSO_H
#define INTERVALPSO_H

# include <INTERVAL/intervalproblem.h>
# include <QVariant>
# include <GE/cprogram.h>
# include <CORE/parameterlist.h>
class IntervalPso
{
private:
    IntervalProblem         *problem;
    IntervalData            fitnessArray;
    IntervalData            bestFitnessArray;
    vector<IntervalData>    particle;
    vector<IntervalData>    bestParticle;
    vector<IntervalData>    velocity;
    IntervalData            bestx;
    Interval                besty;
    double                  inertia;
    ParameterList           plist;
    const   int             nsamples=50;
public:
    IntervalPso(IntervalProblem *p);
    void        setProblem(IntervalProblem *p);
    void        setParam(QString name,QString value,QString help="");
    Interval    fitness(IntervalData &x);
    void        Solve();
    void        getBest(IntervalData &x,Interval &y);
    ParameterList getParameterList() const;
    ~IntervalPso();
};

#endif // INTERVALPSO_H
