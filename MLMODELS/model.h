#ifndef MODEL_H
#define MODEL_H
# include <MLMODELS/dataset.h>
# include <CORE/parameterlist.h>


class Model : public ParameterList
{
protected:
    /**
     * @brief trainDataset, provides the train dataset
     */
    Dataset *trainDataset;
    /**
     * @brief testDataset, provides the test dataset
     */
    Dataset *testDataset;
    bool    noRemoveData=false;
    /**
     * @brief xall, all the training data
     */
    Matrix xall;
    bool fastExpFlag=false;

public:
    /**
     * @brief Model, the default constructor
     */
    Model();
    void        disableRemoveData();
    /**
     * @brief setTrainSet, alters the training dataset
     * @param tr
     */
    void        setTrainSet(Dataset *tr);
    /**
     * @brief setTestSet, alters the testing dataset
     * @param tt
     */
    void        setTestSet(Dataset *tt);
    /**
     * @brief getOutput
     * @param x, the input pattern
     * @return the output of the model for any given pattern
     */
    virtual     double  getOutput(Data &x)=0;
    /**
     * @brief getTrainError
     * @return the training error of the model
     */
    double      getTrainError();
    /**
     * @brief getTestError
     * @param test
     * @return  the test error for a provided dataset
     */
    double      getTestError(Dataset *test) ;
    /**
     * @brief getClassTestError
     * @param test
     * @return  the class test error for a provided dataset
     */
    double      getClassTestError(Dataset *test) ;
    /**
     * @brief getDistance
     * @param x1
     * @param x2
     * @return  the euclidean distance between vectors x1 and x2.
     */
    double      getDistance(Data &x1,Data &x2);
    /**
     * @brief initModel, initialize the parameters of the model.
     */
    virtual     void initModel();
    /**
     * @brief trainModel, trains the model to estimate the best set of parameters.
     */
    virtual     void trainModel();

    /**
     * @brief loadTrainSet, loads the training set from a disk file.
     */
    void        loadTrainSet();
    /**
     * @brief loadTestSet, loads the test set from a disk file.
     */
    void        loadTestSet();
    /**
     * @brief getTestError
     * @return  the test error of the model
     */
    double      getTestError();
    /**
     * @brief getClassTestError
     * @return the class test error of the model
     */
    double      getClassTestError();
    /**
     * @brief getTrainDataset
     * @return  the used train dataset
     */
    Dataset     *getTrainDataset();
    /**
     * @brief testModel, perform test for the model
     * @param trainError
     * @param testError
     * @param classError
     */
    virtual     void  testModel(double &trainError,double &testError,double &classError);

    /**
     * @brief getAverageClassError
     * @param test
     * @return  the class error per clas for the dataset test.
     */
    double  getAverageClassError(Dataset *test);

    void    enableFastExp();
    void    disableFastExp();
    int		printConfusionMatrix(vector<double> &T,vector<double> &O,
                                 vector<double> &precision,
                                 vector<double> &recall);
    void    getPrecisionAndRecall(double &precision,double &recall);
    virtual ~Model();
};

#endif // MODEL_H
