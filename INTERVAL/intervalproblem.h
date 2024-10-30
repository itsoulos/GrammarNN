#ifndef INTERVALPROBLEM_H
#define INTERVALPROBLEM_H
# include <vector>
# include <iostream>
# include <QJsonObject>
# include <INTERVAL/interval.h>
#include <random>
using namespace std;
typedef vector<double> Data;
typedef vector<Interval> IntervalData;

class IntervalProblem
{
protected:
    int                 dimension;
    IntervalData        margin;
    const int     maxSearchDepth=5;

    /**
     * @brief modelSeed, the seed number used in the model initialization.
     */
    int     modelSeed;

    mt19937 generator;
    uniform_int_distribution<> intDistrib;
    uniform_real_distribution<> doubleDistrib;
public:
    IntervalProblem(int d);
    void                setDimension(int d);
    void                setMargins(IntervalData &m);
    IntervalData        getMargins() const;
    int                 getDimension() const;
    bool                isPointIn(Data &x) const;
    bool                isPointIn(IntervalData &x) const;
    IntervalData        createRandomInterval();
    void                randomize(IntervalData &x,int pos);
    virtual             void init(QJsonObject x);
    virtual             void setParameter(QString name,QVariant value);
    virtual double      funmin(Data &x);
    virtual void        granal(Data &x,Data &g);
    virtual QJsonObject done(Data &x);
    bool                lowerValue(Interval &a,Interval &b);
    double              getVolume() const;
    double              getVolume(IntervalData &x) const;
    void                boundInterval(IntervalData &x);
    void                printData(IntervalData &x);
    double              randomDouble();
    /**
     * @brief setModelSeed, alters the used seed for the random number generator
     * @param seed
     */
    void    setModelSeed(int seed);
    /**
     * @brief getModelSeed
     * @return  the used seed for the random number generator
     */
    int     getModelSeed() const;
    virtual Data getSample();

    /**
     * @brief ~Model the used destructor
     */


    virtual             ~IntervalProblem();
};

#endif // INTERVALPROBLEM_H
