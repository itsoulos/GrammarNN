## SCRIPT TO EXECUTE THE GRAMMAR NN 

## DATAPATH: the location of datasets
DATAPATH=~/Desktop/ERGASIES/FeatureConstruction2/datasets/tenfolding/

## PROGRAM: The name of the executable
PROGRAM=./GrammarNN

## DATAFILE: the name of the used dataset
DATAFILE=$1

## MODEL: The name of the used model. Available values: Mlp, Rbf
MODEL=Mlp

## METHOD: The name of the used training method, when the Grammar Genetic finishes. 
#          Avalaible values: Bfgs, Adam, Gradient, Lbfgs, Genetic, Pso
METHOD=Genetic

## LOCAL: The local search optimizer. Available methods: bfgs,lbfgs,adam,gradient
LOCAL=bfgs

# SAMPLER: The used sampling methods. Available values: uniform, triangular,maxwell,kmeans
SAMPLER=uniform

# TERMINATION: The termination method used. Available values: maxiters, doublebox, similarity
TERMINATION=maxiters

OPTPARAMS="--opt_debug=no --opt_localsearch=$LOCAL --opt_sampler=$SAMPLER --opt_termination=$TERMINATION"
if [ $MODEL = "Mlp" ]
then
###NEURAL NETWORK PARAMS
## mlp_nodes:       The number of processing nodes
#  mlp_leftmargin:  The initial left bound of the neural network nodes.
#  mlp_rightmargin: The initial right bound of the neural network nodes.
#  mlp_initmethod:  The method used to initialize the weights. Available values: smallvalues, xavier.
#  model_trainfile: The used trainfile
#  model_testfile:  The used testfile
#  mlp_usebound:    Set it to true, in order to use the bounding techniques.
#  mlp_boundlimit:  The value used when the usebound is active
	MODELPARAMS="--mlp_nodes=10 --mlp_leftmargin=-10 --mlp_rightmargin=10 --mlp_initmethod=smallvalues --model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --mlp_usebound=false --mlp_boundlimit=10.0"
elif [ $MODEL = "Rbf" ]
then
	MODELPARAMS="--opt_method=$METHOD --mlp_nodes=10 --mlp_leftmargin=-10 --mlp_rightmargin=10 --mlp_initmethod=smallvalues --model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --mlp_usebound=false --mlpboundlimit=10.0"
fi


MAINPARAMS="--gnn_iters=1 --gnn_seed=1 --gnn_method=$METHOD --gnn_model=$MODEL"
## Execute the program
 $PROGRAM $MAINPARAMS $OPTPARAMS $MODELPARAMS
