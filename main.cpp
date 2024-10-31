#include <QCoreApplication>
# include <MLMODELS/mlpproblem.h>
# include <MLMODELS/rbfproblem.h>
# include <GE/grammargenetic.h>
# include <METHODS/bfgs.h>
# include <METHODS/lbfgs.h>
# include <METHODS/adam.h>
# include <METHODS/gradientdescent.h>
# include <METHODS/ipso.h>
# include <METHODS/genetic.h>
# include <QDebug>


GrammarGenetic *gen=NULL;
QVector<Parameter> mainParams;
QVector<Optimizer*> method;
QStringList methodName;
Model *selectedModel = NULL;
QStringList modelName;
QVector<Model*> model;
QString selectedTrainFile = "";
QString selectedTestFile = "";
QString selectedModelName = "";
IntervalData bestMargin;

void    loadMethods()
{
    methodName<<"Bfgs";
    method<<new Bfgs;
    methodName<<"Lbfgs";
    method<<new Lbfgs;
    methodName<<"Adam";
    method<<new Adam;
    methodName<<"Genetic";
    method<<new Genetic;
    methodName<<"Pso";
    method<<new iPso;
}


void unloadMethods()
{
    for(int i=0;i<method.size();i++)
        delete method[i];
}

void    loadModels()
{
    modelName<<"Mlp";
    model<<new MlpProblem;
    modelName<<"Rbf";
    model<<new RbfProblem;
}

void    unloadModels()
{
    for(int i=0;i<model.size();i++)
        delete model[i];
}

void makeMainParams()
{
    mainParams<<Parameter("help","","Show help screen");
    mainParams<<Parameter("gnn_method",methodName[0],methodName,"Used Optimization method");
    mainParams<<Parameter("gnn_model",modelName[0],modelName,"Model name. Values: mlp,rbf");
    mainParams<<Parameter("gnn_seed",1,0,100,"Random Seed");
    mainParams<<Parameter("gnn_iters",1,1,100,"Number of iterations");
}

void init()
{
    loadMethods();
    loadModels();
    gen = new GrammarGenetic(200,200,NULL);
    makeMainParams();
}

void done()
{
    if(gen!=NULL)
        delete gen;
    unloadMethods();
    unloadModels();
}

void printOption(Parameter param)
{
    qDebug().noquote()<<"Parameter name:           "<<param.getName();
    qDebug().noquote()<<"\tParameter default value:"<<param.getValue();
    qDebug().noquote()<<"\tParameter purpose:      "<<param.getHelp();
}


void shouldTerminate()
{
    done();
    qApp->exit(0);
    exit(0);
}

void error(QString message)
{
    printf("Fatal error %s \n",message.toStdString().c_str());
    shouldTerminate();
}

void    printHelp()
{
    qDebug().noquote()<<"MAIN PARAMETERS\n=================================================";
    for(int i=0;i<mainParams.size();i++)
        printOption(mainParams[i]);
    for(int i=0;i<method.size();i++)
    {
        qDebug().noquote()<<"METHOD :"<<methodName[i]<<"PARAMETERS\n=================================================";
        ParameterList pt = method[i]->getParameterList();
        for(int j=0;j<pt.countParameters();j++)
        {
            Parameter pa =pt.getParam(j);
            printOption(pa);
        }
    }
    for(int i=0;i<model.size();i++)
    {
        qDebug().noquote()<<"MODEL NAME :"<<modelName[i]<<"PARAMETERS\n=================================================";
        for(int j=0;j<model[i]->countParameters();j++)
        {
            Parameter pa = model[i]->getParam(j);
            printOption(pa);
        }
    }

    qDebug().noquote()<<"Grammar Genetic PARAMETERS\n=================================================";
    ParameterList pg = gen->getParameterList();
    for(int i=0;i<pg.countParameters();i++)
    {
        Parameter pa = pg.getParam(i);
        printOption(pa);
    }
    shouldTerminate();

}

void parseCmdLine(QStringList args)
{
    QString lastParam="";
    for(int i=1;i<args.size();i++)
    {
        if(args[i]=="--help") printHelp();
        QStringList lst = args[i].split("=");
        if(lst.size()<=1)
            error(QString("Fatal error %1 not an option").arg(args[i]));
        QString name = lst[0];
        QString value = lst[1];
        if(name.startsWith("--"))
            name = name.mid(2);
        if(value=="")
        {
            error(QString("Param %1 is empty.").arg(value));
        }
        bool foundParameter = false;
        //check in mainParams
        for(int i=0;i<mainParams.size();i++)
        {
            if(mainParams[i].getName()==name)
            {
                mainParams[i].setValue(value);
                foundParameter=true;
                break;
            }
        }
        if(foundParameter) continue;
        //check in methods
        for(int j=0;j<method.size();j++)
        {
            ParameterList pt = method[j]->getParameterList();
            if(pt.contains(name))
            {
                pt.setParam(name,value);
                method[j]->setParam(name,value);
                foundParameter= true;
                break;
            }
        }
        if(foundParameter) continue;
        //check in models
        for(int j=0;j<model.size();j++)
        {
           if(model[j]->contains(name))
           {
               model[j]->setParam(name,value);
               foundParameter = true;
               break;
           }
        }
        if(foundParameter) continue;
        //check in genetic
        ParameterList pg = gen->getParameterList();
        if(pg.contains(name))
        {
            pg.setParam(name,value);
            gen->setParam(name,value);
            foundParameter=true;
        }
        if(!foundParameter)
            error(QString("Parameter %1 not found.").arg(name));
    }
}


void    runFirstPhase()
{
    //phase 1. Run a small genetic algorithm to identify the most promising
    Genetic gd;
    gd.setParam("opt_debug","no");
    gd.setProblem(dynamic_cast<IntervalProblem*>(selectedModel));
    gd.solve();
    Data xx;
    xx.resize(dynamic_cast<IntervalProblem*>(selectedModel)->getDimension());
    double yy;
    gd.getBest(xx,yy);
    printf("PHASE 1. Best value=%20.10lg\n",yy);
    bestMargin.resize(xx.size());
    for(int i=0;i<(int)xx.size();i++)
    {
        bestMargin[i]=Interval(-xx[i],xx[i]);
    }
    dynamic_cast<IntervalProblem*>(selectedModel)->setMargins(bestMargin);
}

void    loadDataFiles()
{
    for(int i=0;i<mainParams.size();i++)
    {
        if(mainParams[i].getName()=="gnn_model")
        {
            selectedModelName=mainParams[i].getValue();
            break;
        }
    }

    for(int i=0;i<modelName.size();i++)
    {
        if(modelName[i]==selectedModelName)
        {
            selectedModel = model[i];
            break;
        }
    }

    //load train and test files
    selectedTrainFile = selectedModel->getParam("model_trainfile").getValue();
    selectedTestFile =  selectedModel->getParam("model_testfile").getValue();

    if(selectedTrainFile.isEmpty() || selectedTestFile.isEmpty())
        error("The user should provide train and test file");

    if(!QFile::exists(selectedTrainFile))
        error(QString("Trainfile %1 does not exist").arg(selectedTrainFile));

    if(!QFile::exists(selectedTestFile))
        error(QString("Trainfile %1 does not exist").arg(selectedTestFile));
    selectedModel->loadTrainSet();
    selectedModel->loadTestSet();
    selectedModel->initModel();
}

void    runSecondPhase()
{
    gen->setProblem(dynamic_cast<IntervalProblem*>(selectedModel));
    gen->Solve();
}


void    runThirdPhase()
{
    //evaluate the new margins
}

void    run()
{
    loadDataFiles();
    runFirstPhase();
    runSecondPhase();
    runThirdPhase();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);
    setlocale(LC_ALL,"C");
    init();
    parseCmdLine(app.arguments());
    run();
    done();
    return 0;
}
