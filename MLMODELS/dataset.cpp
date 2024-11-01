#include "dataset.h"

Dataset::Dataset()
{
    clearPoints();
}

Dataset::Dataset(IntervalProblem *p,int N)
{
    xpoint.resize(N);
    ypoint.resize(N);
    for(int i=0;i<N;i++)
    {
        xpoint[i].resize(p->getDimension());
        xpoint[i]=p->getSample();
        ypoint[i]=p->funmin(xpoint[i]);
    }
    makeClassVector();
}

void    Dataset::makeClassVector()
{
    classVector.clear();
    for(int i=0;i<(int)ypoint.size();i++)
    {
        int imin = -1;
        for(int j=0;j<classVector.size();j++)
        {
            if(fabs(classVector[j]-ypoint[i])<1e-6)
            {
                imin = j;
                break;
            }
        }
        if(imin==-1)
        {

            classVector.push_back(ypoint[i]);
        }
    }
}

void    Dataset::loadFromDataFile(QString filename)
{
    QFile fp(filename);
    if(!fp.open(QIODevice::ReadOnly|QIODevice::Text))
        return;
    int d=0,c=0;
    QTextStream st(&fp);
    st>>d;
    st>>c;
    Data xp;
    xp.resize(d);
    double yp;
    for(int i=0;i<c;i++)
    {
        for(int j=0;j<d;j++)
        {
            double x;
            st>>x;
            xp[j]=x;
        }
        st>>yp;
        xpoint.push_back(xp);
        ypoint.push_back(yp);
    }
    fp.close();
    makeClassVector();
}

void    Dataset::loadFromArffFile(QString filename)
{
    QFile fp(filename);
    if(!fp.open(QIODevice::ReadOnly|QIODevice::Text))
        return;
    int d=0,c=0;
    bool startData = false;
    QTextStream st(&fp);
    QString line;
    QStringList items;
    QString relationName;
    Data xp;
    double yp;
    do
    {
        st>>line;
        if(st.atEnd()) break;
        if(line.size()<3) continue;
        if(line.startsWith("@Relation"))
        {
            items = line.split(" ");
            relationName= items[1];
            continue;
        }
        if(line.startsWith("@ATTRIBUTE")) continue;
        if(line=="@DATA")
        {
            startData=true;
            continue;
        }
        if(startData)
        {
            items = line.split(",");
            if(d==0)
            {
                d=items.size()-1;
                xp.resize(d);
            }
            if(items.size()==d+1)
            {
                for(int i=0;i<d;i++)
                    xp[i]=items[i].toDouble();
                yp = items[d].toDouble();
                xpoint.push_back(xp);
                ypoint.push_back(yp);
            }
        }
    }while(true);
    fp.close();
    makeClassVector();
}

void    Dataset::loadFromCsvFile(QString filename)
{
    QFile fp(filename);
    if(!fp.open(QIODevice::ReadOnly|QIODevice::Text))
        return;
    int d=0,c=0;
    QTextStream st(&fp);
    QString line;
    QStringList items;
    Data xp;
    double yp;

    do
    {
        st>>line;

        if(st.atEnd()) break;
        if(line.size()<3) continue;
        items = line.split(",");
        if(items.size()<2) continue;

        if(d==0)
        {
            d=items.size()-1;
            xp.resize(d);
        }

        if(items.size()!=d+1) continue;

        for(int i=0;i<d;i++)
            xp[i]=items[i].toDouble();
        yp = items[d].toDouble();
        xpoint.push_back(xp);
        ypoint.push_back(yp);
    }while(true);
    fp.close();
    makeClassVector();

}

Data    &Dataset::getXpoint(int pos)
{
    return xpoint[pos];
}

double  Dataset::getYpoint(int pos) const
{
    return ypoint[pos];
}

int     Dataset::dimension() const
{
    if(xpoint.size()==0)
        return 0;
    return xpoint[0].size();
}

int     Dataset::count() const
{
    return xpoint.size();
}

void    Dataset::clearPoints()
{
    xpoint.clear();
    ypoint.clear();
    classVector.clear();
}

void    Dataset::addPoint(Data &x,double y)
{
    xpoint.push_back(x);
    ypoint.push_back(y);
    makeClassVector();
}

void    Dataset::getPoint(int pos,Data &x,double y)
{
    x = xpoint[pos];
    y = ypoint[pos];
}

double  Dataset::getClass(int pos)
{
    return estimateClass(ypoint[pos]);
}

double  Dataset::estimateClass(double value)
{
    if(classVector.size()==0)
        return -1e+10;
    int minIndex =0;
    double minValue=fabs(classVector[0]-value);
    for(int i=0;i<(int)classVector.size();i++)
    {
        if(fabs(classVector[i]-value)<minValue)
        {
            minValue = fabs(classVector[i]-value);
            minIndex = i;
        }
    }

    return classVector[minIndex];
}

Matrix Dataset::getAllXpoint() const
{
    return xpoint;
}

Dataset::~Dataset()
{
    //nothing here
}
