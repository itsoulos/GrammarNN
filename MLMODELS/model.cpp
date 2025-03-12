#include "model.h"

Model::Model()
{
    trainDataset = NULL;
    testDataset = NULL;
    addParam(Parameter("model_trainfile","","The trainfile used"));
    addParam(Parameter("model_testfile","","The test file used"));
    QStringList format;
    format<<"data"<<"csv"<<"arff";
    addParam(Parameter("model_dataformat",format[0],format,"Data format used in datafiles"));
    fastExpFlag = true;
}

void    Model::trainModel()
{
}


void    Model::testModel(double &trainError,double &testError,double &classError)
{
        trainError = getTrainError();
        testError  = getTestError();
        classError = getClassTestError();
}

void    Model::initModel()
{

}

void        Model::loadTrainSet()
{
    QString tr = getParam("model_trainfile").getValue();
    QString format=getParam("model_dataformat").getValue();
    if(tr=="") return;
    if(trainDataset!=NULL) delete trainDataset;
    trainDataset = new Dataset();
    if(format=="data")
        trainDataset->loadFromDataFile(tr);
    else
    if(format=="csv")
        trainDataset->loadFromCsvFile(tr);
    else
        trainDataset->loadFromArffFile(tr);
    xall = trainDataset->getAllXpoint();
}

void        Model::loadTestSet()
{
    QString tr = getParam("model_testfile").getValue();
    QString format=getParam("model_dataformat").getValue();
    if(tr=="") return;
    if(testDataset!=NULL) delete testDataset;
    testDataset = new Dataset();
    if(format=="data")
        testDataset->loadFromDataFile(tr);
    else
    if(format=="csv")
        testDataset->loadFromCsvFile(tr);
    else
        testDataset->loadFromArffFile(tr);
}

double      Model::getTestError()
{
    return getTestError(testDataset);
}

double      Model::getClassTestError()
{
    return getClassTestError(testDataset);
}

double      Model::getDistance(Data &x1,Data &x2)
{
    double sum = 0.0;
    for(int i=0;i<x1.size();i++) sum+=(x1[i]-x2[i])*(x1[i]-x2[i]);
    return sqrt(sum);
}

void    Model::setTestSet(Dataset *tt)
{
    testDataset = tt;
}

void        Model::setTrainSet(Dataset *tr)
{
    trainDataset = tr;
    xall = trainDataset->getAllXpoint();
}

double  Model::getTrainError()
{
    double error = 0.0;
    int xallsize  = xall.size();
    for(int i=0;i<xallsize;i++)
    {
        Data xx = xall[i];
        double yy = trainDataset->getYpoint(i);
        double per = getOutput(xx);
        error+= (per-yy)*(per-yy);
    }
    return error;
}


void    Model::enableFastExp()
{
    fastExpFlag = true;
}
void    Model::disableFastExp()
{
    fastExpFlag = false;
}

/** kanei oti kai i getTrainError() alla gia to test set **/
double  Model::getTestError(Dataset *test)
{
    double error = 0.0;
    for(int i=0;i<test->count();i++)
    {
        Data xx = test->getXpoint(i);
        double yy = test->getYpoint(i);
        double per = getOutput(xx);
        error+= (per-yy)*(per-yy);
    }
    return error;
}

/** edo epistrefo to classification sfalma sto test set **/
double  Model::getClassTestError(Dataset *test)
{
    double error = 0.0;
    for(int i=0;i<test->count();i++)
    {
        Data xx = test->getXpoint(i);
        double realClass = test->getYpoint(i);
        double per = getOutput(xx);
        double estClass = test->estimateClass(per);

        error+= (fabs(estClass - realClass)>1e-5);
    }
    /** to metatrepoume se pososto **/
    return error*100.0/test->count();
}
void        Model::disableRemoveData()
{
    noRemoveData=true;
}

Dataset     *Model::getTrainDataset()
{
    return trainDataset;
}



int	Model::printConfusionMatrix(vector<double> &T,vector<double> &O,
                             vector<double> &precision,
                             vector<double> &recall)
{
    int i,j;

    int total_class = 0;
    int N=T.size();
    Data dclass = trainDataset->getClassVector();
    int nclass=dclass.size();
    total_class = nclass;
    precision.resize(nclass);
    recall.resize(nclass);
    int icount = 0;
    int **CM;
    //printf("** CONFUSION MATRIX ** Number of classes: %d\n",nclass);
    CM=new int*[nclass];
    for(i=0;i<nclass;i++) CM[i]=new int[nclass];
    for(i=0;i<nclass;i++)
        for(j=0;j<nclass;j++) CM[i][j] = 0;

    for(i=0;i<N;i++) CM[(int)T[i]][(int)O[i]]++;

    for(i=0;i<nclass;i++)
    {
        double sum = 0.0;
        for(j=0;j<nclass;j++)
            sum+=CM[j][i];
	if(sum==0.0)
	{
		total_class--;
		continue;
	}

        precision[icount]=CM[i][i]/sum;
        sum = 0.0;
        for(j=0;j<nclass;j++)
            sum+=CM[i][j];
        recall[icount]=CM[i][i]/sum;
	icount++;
    }
    for(i=0;i<nclass;i++)
    {
        for(j=0;j<nclass;j++)
        {
            //printf("%4d ",CM[i][j]);
        }
        //printf("\n");
        delete[] CM[i];
    }
    delete[] CM;
    return total_class;
}

void    Model::getPrecisionAndRecall(double &precision,double &recall)
{
    vector<double> T;
    vector<double> O;
    vector<double> dprecision;
    vector<double> drecall;
    vector<double> dclass = trainDataset->getClassVector();
    int tsize = testDataset->count();
    T.resize(tsize);
    O.resize(tsize);
    precision = 0.0;
    recall    = 0.0;
    for(int i=0;i<tsize;i++)
    {
        Data pattern = testDataset->getXpoint(i);
        double realOutput = testDataset->getYpoint(i);
        double y = getOutput(pattern);
        T[i]=testDataset->estimateClassIndex(realOutput);
        O[i]=testDataset->estimateClassIndex(y);
    }
    int d = printConfusionMatrix(T,O,dprecision,drecall);
    for(int i=0;i<d;i++)
    {
        precision+=dprecision[i];
        recall+=drecall[i];

    }
    precision/=d;
    recall/=d;
}

Model::~Model()
{
if(noRemoveData) return;
    if(trainDataset!=NULL)
    {
        delete trainDataset;
        trainDataset = NULL;
        }
    if(testDataset!=NULL)
    {
        delete testDataset;
        testDataset = NULL;
        }
}
