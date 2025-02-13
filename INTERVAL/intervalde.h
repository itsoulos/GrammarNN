#ifndef INTERVALDE_H
#define INTERVALDE_H
# include <INTERVAL/intervalproblem.h>
# include <QVariant>
# include <GE/cprogram.h>
# include <CORE/parameterlist.h>

class IntervalDE
{
private:
    IntervalProblem     *problem;
    IntervalData        fitnessArray;
    vector<IntervalData> agent;
    int NP;
    double F,CR;
    ParameterList plist;
    const int nsamples=50;
    Data drandDat;
    Data ff;
public:
    IntervalDE(IntervalProblem *p);
    void    setProblem(IntervalProblem *p);
    void        setParam(QString name,QString value,QString help="");
    Interval    fitness(IntervalData &x);
    void        Solve();
    void        getBest(IntervalData &x,Interval &y);
    double  getAdaptiveWeight(int iter);
    void    calculateMigrantWeights();
    ParameterList getParameterList() const;

    ~IntervalDE();
};

#endif // INTERVALDE_H
