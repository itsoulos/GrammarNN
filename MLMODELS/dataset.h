#ifndef DATASET_H
#define DATASET_H
# include <math.h>
# include <QString>
# include <QStringList>
# include <QFile>
# include <QTextStream>
# include <vector>
# include <iostream>
using namespace std;

typedef vector<double> Data;
typedef vector<Data> Matrix;

class Dataset
{
private:
    Matrix xpoint;
    Data   ypoint;
    Data   classVector;
    void    makeClassVector();
public:
    Dataset();
    void    loadFromDataFile(QString filename);
    void    loadFromArffFile(QString filename);
    void    loadFromCsvFile(QString filename);
    Data    &getXpoint(int pos);
    double  getYpoint(int pos) const;
    int     dimension() const;
    int     count() const;
    void    clearPoints();
    void    addPoint(Data &x,double y);
    void    getPoint(int pos,Data &x,double y);
    double  getClass(int pos);
    double  estimateClass(double value);
    Matrix getAllXpoint() const;
    ~Dataset();
};

#endif // DATASET_H
