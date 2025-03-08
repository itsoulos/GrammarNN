## SCRIPT TO EXECUTE THE GRAMMAR NN 

## DATAPATH: the location of datasets
DATAPATH=~/Desktop/ERGASIES/FeatureConstruction2/datasets/tenfolding/

## PROGRAM: The name of the executable
PROGRAM=./GrammarNN

## DATAFILE: the name of the used dataset
DATAFILE=$1

## INTERVALMETHOD: The used interval method. Available values: grammar, intervalde, intervalpso, intervalanneal
INTERVALMETHOD=intervalanneal

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

OPTPARAMS="--opt_debug=yes --opt_localsearch=$LOCAL --opt_sampler=$SAMPLER --opt_termination=$TERMINATION"
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
	MODELPARAMS="--mlp_nodes=10 --mlp_leftmargin=-10 --mlp_rightmargin=10 --mlp_initmethod=smallvalues --model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --mlp_usebound=true --mlp_boundlimit=10.0"
elif [ $MODEL = "Rbf" ]
then
###RBF NETWORK PARAMS
## rbf_nodes: The number of processing nodes
## rbf_factor: The scale factor used for the margins
	MODELPARAMS="--rbf_nodes=10 --rbf_factor=2 --model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test"
fi


### MAIN PROGRAM PARAMETERS
## gnn_iters:  The number of iterations for simulation experiments.
## gnn_firstphase: Enable or disable the first phase (yes,no)
## gnn_seed:   The seed used in the random number generator.
## gnn_method: The optimization method used in the final phase of the simulation experiments to evaluate the bounds. 
## gnn_model:  The model used during the experiments
MAINPARAMS="--gnn_intervalmethod=$INTERVALMETHOD --gnn_firstphase=no --gnn_iters=10 --gnn_seed=1 --gnn_method=$METHOD --gnn_model=$MODEL"

### Grammar Genetic PARAMETERS
## ggen_count:  The number of chromosomes used in the Grammar Genetic process.
## ggen_size:   The size of the chromosomes used.
## ggen_srate:  The selection rate for the Grammar Genetic
## ggen_mrate:  The mutation rate for the Grammar Genetic
## ggen_gens:   The maximum number of allowed generations for Grammar Genetic
GENPARAMS="--ggen_count=500 --ggen_size=100 --ggen_srate=0.1 --ggen_mrate=0.05 --ggen_gens=200"

### IntervalDE PARAMETERS
## ide_np: 		The number of agents. Default 200
## ide_f : 		The fixed value used for differential weight. Default 0.8
## ide_cr: 		The crossover rate. Default 0.9
## ide_weightmethod:    The weight method used. Available values: random, fixed, adapt, migrant. Default: random
## ide_maxiters:	The maximum number of allowed iterations. Default 200.

IDEPARAMS="--ide_np=200 --ide_f=0.8 --ide_cr=0.9 --ide_weightmethod=random --ide_maxiters=200"
## Execute the program
 $PROGRAM $GENPARAMS $IDEPARAMS $MAINPARAMS $OPTPARAMS $MODELPARAMS
