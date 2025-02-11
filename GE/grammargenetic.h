#ifndef GRAMMARGENETIC_H
#define GRAMMARGENETIC_H
# include <INTERVAL/intervalproblem.h>
# include <QVariant>
# include <GE/cprogram.h>
# include <CORE/parameterlist.h>
typedef vector<int> IDATA;


/** a typical program (x2,0,1),(x3,1,1),(x1,0,0),(x2,1,1)*/
class GrammarGenetic
{
private:
    IntervalProblem *problem;
    vector<IDATA> chromosome;
    vector<IDATA> children;
    IntervalData fitnessArray;
    Cprogram *program;
    const int nsamples=50;
    int     getParent();
    double selection_rate;
    double mutation_rate;
    void calcFitnessArray();
    void select();
    void crossover();
    void mutate();
    int generation;
    int maxGenerations;
    int gcount;
    int gsize;
    Data drandDat;
    int MAX_RULE;
    ParameterList plist;
public:
    GrammarGenetic(int count,int size,IntervalProblem *p);
    void    setProblem(IntervalProblem *p);
    void    setParam(QString name,QString value,QString help="");
    Interval fitness(IDATA &genome);
    void    setSelectionRate(double r);
    void    setMutationRate(double r);
    void    setMaxGenerations(int m);
    double  getSelectionRate() const;
    double  getMutationRate() const;
    int     getMaxGenerations() const;
    void    nextGeneration();
    void    Solve();
    void    getBest(IntervalData &x,Interval &y);
    Data    getBestPoint();
    void    localSearch(int pos);
    ParameterList getParameterList() const;
    ~GrammarGenetic();
};

#endif // GRAMMARGENETIC_H
