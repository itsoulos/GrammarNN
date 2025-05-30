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
# include <INTERVAL/intervalde.h>
# include <INTERVAL/intervalpso.h>
# include <INTERVAL/intervalanneal.h>
# include <QDebug>


GrammarGenetic *gen=NULL;
IntervalDE *ide=NULL;
IntervalPso *ipso=NULL;
IntervalAnneal *isiman=NULL;
QVector<Optimizer*> method;
QStringList methodName;
Model *selectedModel = NULL;
QStringList modelName;
QVector<Model*> model;
QString selectedMethodName ="";
Optimizer *selectedMethod = NULL;
QString selectedTrainFile = "";
QString selectedTestFile = "";
QString selectedModelName = "";
IntervalData bestMargin;
ParameterList mainParamList;
double average_train_error = 0.0;
double average_test_error  = 0.0;
double average_class_error = 0.0;
double average_precision = 0.0;
double average_recall    = 0.0;
int gnn_iters = 0;
QString gnn_intervalmethod="intervalde";

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
    mainParamList.addParam(Parameter("help","","Show help screen"));
    QStringList m;
    m<<"intervalde"<<"grammar"<<"intervalpso"<<"intervalanneal"<<"inone";
    mainParamList.addParam(Parameter("gnn_intervalmethod",m[0],m,"The used interval method"));
    QStringList yesno;
    yesno<<"yes"<<"no";
    mainParamList.addParam(Parameter("gnn_firstphase",yesno[0],yesno,"Enable or disable first phase (yes,no)"));
    mainParamList.addParam(Parameter("gnn_method",methodName[0],methodName,"Used Optimization method"));
    mainParamList.addParam(Parameter("gnn_model",modelName[0],modelName,"Model name. Values: mlp,rbf"));
    mainParamList.addParam(Parameter("gnn_seed",1,0,100,"Random Seed"));
    mainParamList.addParam(Parameter("gnn_iters",1,1,100,"Number of iterations"));
}

void init()
{
    loadMethods();
    loadModels();
    gen = new GrammarGenetic(200,200,NULL);
    ide = new IntervalDE(NULL);
    ipso = new IntervalPso(NULL);
    isiman = new IntervalAnneal(NULL);
    makeMainParams();
}

void done()
{
    if(gen!=NULL)
        delete gen;
    if(ide!=NULL)
        delete ide;
    if(ipso!=NULL)
        delete ipso;
    if(isiman!=NULL)
        delete isiman;
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
    for(int i=0;i<mainParamList.countParameters();i++)
        printOption(mainParamList.getParam(i));
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
        if(mainParamList.contains(name))
        {
            mainParamList.setParam(name,value);
            foundParameter=true;
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
        //check in IntervalDE
        pg = ide->getParameterList();
        if(pg.contains(name))
        {
            pg.setParam(name,value);
            ide->setParam(name,value);
            foundParameter=true;
        }
        //check in IntervalPSO
        pg = ipso->getParameterList();
        if(pg.contains(name))
        {
            pg.setParam(name,value);
            ipso->setParam(name,value);
            foundParameter=true;
        }

        //check in IntervalAnneal
        pg = isiman->getParameterList();
        if(pg.contains(name))
        {
            pg.setParam(name,value);
            isiman->setParam(name,value);
            foundParameter=true;
        }
        if(!foundParameter)
            error(QString("Parameter %1 not found.").arg(name));
    }
}


void    runFirstPhase()
{
    Data xx;
    xx.resize(dynamic_cast<IntervalProblem*>(selectedModel)->getDimension());
    bestMargin.resize(dynamic_cast<IntervalProblem*>(selectedModel)->getDimension());

   QString yesno = mainParamList.getParam("gnn_firstphase").getValue();
   if(yesno=="no")
   {

       if(selectedModelName=="Rbf")
        {
            selectedModel->initModel();
            bestMargin = dynamic_cast<RbfProblem*>(selectedModel)->getMargins();
            return;
        }
       else
       if(selectedModelName=="Mlp")
       {
           selectedModel->initModel();
           bestMargin = dynamic_cast<MlpProblem*>(selectedModel)->getMargins();
           return;
       }
   }


   if(selectedModelName=="Rbf") {
       selectedModel->initModel();
       selectedModel->disableFastExp();
   }

    //phase 1. Run a small genetic algorithm to identify the most promising
    Genetic gd;
    gd.setParam("opt_debug","yes");
    gd.setProblem(dynamic_cast<IntervalProblem*>(selectedModel));
    gd.solve();

    double yy;
    gd.getBest(xx,yy);
    printf("PHASE 1. Best value=%20.10lg\n",yy);
    
    for(int i=0;i<(int)bestMargin.size();i++)
    {
        bestMargin[i]=Interval(-2.0 * fabs(xx[i]), 2.0*fabs(xx[i]));
    }



}

void    loadDataFiles()
{
    selectedModelName = mainParamList.getParam("gnn_model").getValue();


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
    selectedModel->disableFastExp();
}

void    runSecondPhase()
{
    gnn_intervalmethod = mainParamList.getParam("gnn_intervalmethod").getValue();
	selectedModel->setParam("mlp_usebound","false");
    selectedModel->setParam("rbf_usebound","false");

    selectedModel->enableFastExp();
    selectedModel->initModel();
    Interval yy;
    dynamic_cast<IntervalProblem*>(selectedModel)->setMargins(bestMargin);
    if(gnn_intervalmethod=="grammar")
    {
        gen->setProblem(dynamic_cast<IntervalProblem*>(selectedModel));
        gen->Solve();
        gen->getBest(bestMargin,yy);
    }
    else
    if(gnn_intervalmethod=="intervalde")
    {
        ide->setProblem(dynamic_cast<IntervalProblem*>(selectedModel));
        ide->Solve();
        ide->getBest(bestMargin,yy);
    }
    else
    if(gnn_intervalmethod=="intervalpso")
    {
        ipso->setProblem(dynamic_cast<IntervalProblem*>(selectedModel));
        ipso->Solve();
        ipso->getBest(bestMargin,yy);
    }
    else
    if(gnn_intervalmethod=="inone")
    {
        //nothing here
    }
   /* else
    {
        isiman->setProblem(dynamic_cast<IntervalProblem*>(selectedModel));
        isiman->Solve();
        isiman->getBest(bestMargin,yy);
    }*/


    qDebug()<<"PHASE 2. Best interval located: "<<"[ "<<
              yy.leftValue()<<","<<yy.rightValue()<<"]";
    dynamic_cast<IntervalProblem*>(selectedModel)->setMargins(bestMargin);
}



void makeReport()
{
    qDebug().noquote() <<"Selected Model:         "<<selectedModelName;
    qDebug()<<qSetRealNumberPrecision( 10 ) <<"Average training error: "<<average_train_error/gnn_iters;
    qDebug()<<qSetRealNumberPrecision( 10 ) <<"Average testing  error: "<<average_test_error/gnn_iters;
    qDebug()<<qSetRealNumberPrecision( 10 ) <<"Average class    error: "<<average_class_error/gnn_iters;
    qDebug()<<qSetRealNumberPrecision( 10 ) <<"Average precision     : "<<average_precision/gnn_iters;
    qDebug()<<qSetRealNumberPrecision( 10 ) <<"Average recall        : "<<average_recall/gnn_iters;
}

void    runThirdPhase()
{
    //evaluate the new margins
    selectedModel->disableFastExp();
	selectedModel->setParam("mlp_usebound","false");
    gnn_iters = mainParamList.getParam("gnn_iters").getValue().toInt();
    selectedMethodName = mainParamList.getParam("gnn_method").getValue();

    qDebug().noquote()<<"Running method "<<selectedMethodName;
    for(int i=0;i<methodName.size();i++)
    {
        if(methodName[i]==selectedMethodName)
        {
            selectedMethod = method[i];
            break;
        }
    }

    for(int ik=1;ik<=gnn_iters;ik++)
    {
        //train model
        selectedMethod->setProblem(dynamic_cast<IntervalProblem*>(selectedModel));
        selectedModel->initModel();
    	dynamic_cast<IntervalProblem*>(selectedModel)->setMargins(bestMargin);
        selectedMethod->solve();
        double tr=0.0,tt=0.0,tc=0.0;
        selectedModel->testModel(tr,tt,tc);
        printf("OPT RUN[%d]= %lf %lf %lf \n",ik,tr,tt,tc);
        average_train_error+=tr;
        average_test_error+=tt;
        average_class_error+=tc;
        double precision = 0.0,recall=0.0;
        selectedModel->getPrecisionAndRecall(precision,recall);
        average_precision+=precision;
        average_recall+=recall;
    }
    makeReport();
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
