#include "lbfgs.h"

Lbfgs::Lbfgs()
{
    haveInitialized = false;
    addParam(Parameter("lbfgs_iters",2001,1,10000,"Maximum number of iterations"));
}

void    Lbfgs::init()
{

}

void    Lbfgs::step()
{
    if(!haveInitialized)
    {
        xpoint.resize(myProblem->getDimension());
        xpoint = myProblem->getSample();
        ypoint = myProblem->funmin(xpoint);
    }
    haveInitialized = false;
    integer iprint=0;
    double factr=1e7;
    double pgtol = 1e-5;
    integer n=myProblem->getDimension();
    integer m=5;
    integer *nbd=new integer[n];
    double *l=new double[n];
    double *u=new double[n];
    double *g=new double[n];
    Data gg;
    gg.resize(n);
    IntervalData mm = myProblem->getMargins();

    for(int i=0;i<n;i++)
    {
        nbd[i]=2;
        l[i]=mm[i].leftValue();
        u[i]=mm[i].rightValue();
    }
    integer taskValue;
    integer *task=&taskValue;
    *task = (integer)START;
    double f;
    const int mmax=20;
    double *wa=new double[(2*mmax+5)*n+11*mmax*11*mmax+8*mmax];
    integer *iwa=new integer[3*n];
    integer csaveValue;
    integer *csave=&csaveValue;
    double dsave[29];
    integer isave[44];
    logical lsave[4];
    double *xx=new double[n];
    for(int i=0;i<n;i++) xx[i]=xpoint[i];
    int iters = 0;
L111:
    setulb(&n, &m, xx, l, u, nbd, &f, g, &factr, &pgtol, wa, iwa, task, &
                                                                        iprint, csave, lsave, isave, dsave);
    if ( IS_FG(*task) ) {
        for(int i=0;i<n;i++) xpoint[i]=xx[i];
        f=myProblem->funmin(xpoint);
        //printf("LBFGS: %10.5lf TASK: %d\n",f,*task);
        myProblem->granal(xpoint,gg);
        for(int i=0;i<n;i++)
            g[i]=gg[i];
        goto L111;
    }
    iters++;
    if ( *task==NEW_X || iters<50 ) {
        goto L111;
    }
    for(int i=0;i<n;i++) xpoint[i]=xx[i];
    delete[] xx;
    delete[] iwa;
    delete[] wa;
    delete[] g;
    delete[] l;
    delete[] u;
    delete[] nbd;
    ypoint = myProblem->funmin(xpoint);
}

bool    Lbfgs::terminated()
{
    return true;
}

void    Lbfgs::setPoint(Data &x,double &y)
{
    haveInitialized = true;
    xpoint = x;
    ypoint = y;
}

void    Lbfgs::getPoint(Data &x,double &y)
{
    x = xpoint;
    y = ypoint;
}

Lbfgs::~Lbfgs()
{
    //nothing here
}

static double c_b7 = 0.;
static integer c__1 = 1;

/* SRB: note that task_len is no longer used since task
 * is now integer* not char*.
 * Similarly for csave (now integer*) and so csave_len not used */

/* Subroutine */ int Lbfgs::mainlb(integer *n, integer *m, double *x,
                                   double *l, double *u, integer *nbd, double *f, double
                                       *g, double *factr, double *pgtol, double *ws, double *
                                       wy, double *sy, double *ss, double *wt, double *wn,
                                   double *snd, double *z__, double *r__, double *d__,
                                   double *t, double *xp, double *wa, integer *index,
                                   integer *iwhere, integer *indx2, integer *task, integer *iprint,
                                   integer *csave, logical *lsave, integer *isave, double *dsave) /*(ftnlen ) */
/* 	task_len, ftnlen csave_len) */
{

    /* System generated locals */
    integer ws_dim1, ws_offset, wy_dim1, wy_offset, sy_dim1, sy_offset,
        ss_dim1, ss_offset, wt_dim1, wt_offset, wn_dim1, wn_offset,
        snd_dim1, snd_offset, i__1=0;
    double d__1, d__2;
    FILE* o__1=NULL;

    /* Local variables */
    integer i__, k;
    double gd, dr, rr, dtd;
    integer col;
    double tol;
    logical wrk;
    double stp, cpu1, cpu2;
    integer head;
    double fold;
    integer nact;
    double ddum;
    integer info, nseg;
    double time;
    integer nfgv, ifun, iter;
    integer wordTemp;
    integer *word=&wordTemp;
    double time1, time2;
    integer iback;
    double gdold;
    integer nfree;
    logical boxed;
    integer itail;
    double theta;
    double dnorm;
    integer nskip, iword;
    double xstep, stpmx;
    integer ileave;
    double cachyt;
    integer itfile;
    double epsmch;
    logical updatd;
    double sbtime;
    logical prjctd;
    integer iupdat;
    double sbgnrm;
    logical cnstnd;
    integer nenter;
    double lnscht;
    integer nintol;

    /* -jlm-jn */
    /*     ************ */

    /*
    Subroutine mainlb

    This subroutine solves bound constrained optimization problems by
      using the compact formula of the limited memory BFGS updates.

    n is an integer variable.
      On entry n is the number of variables.
      On exit n is unchanged.

    m is an integer variable.
      On entry m is the maximum number of variable metric
         corrections allowed in the limited memory matrix.
      On exit m is unchanged.

    x is a double precision array of dimension n.
      On entry x is an approximation to the solution.
      On exit x is the current approximation.

    l is a double precision array of dimension n.
      On entry l is the lower bound of x.
      On exit l is unchanged.

    u is a double precision array of dimension n.
      On entry u is the upper bound of x.
      On exit u is unchanged.

    nbd is an integer array of dimension n.
      On entry nbd represents the type of bounds imposed on the
        variables, and must be specified as follows:
        nbd(i)=0 if x(i) is unbounded,
               1 if x(i) has only a lower bound,
               2 if x(i) has both lower and upper bounds,
               3 if x(i) has only an upper bound.
      On exit nbd is unchanged.

    f is a double precision variable.
      On first entry f is unspecified.
      On final exit f is the value of the function at x.

    g is a double precision array of dimension n.
      On first entry g is unspecified.
      On final exit g is the value of the gradient at x.

    factr is a double precision variable.
      On entry factr >= 0 is specified by the user.  The iteration
        will stop when

        (f^k - f^{k+1})/max{|f^k|,|f^{k+1}|,1} <= factr*epsmch

        where epsmch is the machine precision, which is automatically
        generated by the code.
      On exit factr is unchanged.

    pgtol is a double precision variable.
      On entry pgtol >= 0 is specified by the user.  The iteration
        will stop when

                max{|proj g_i | i = 1, ..., n} <= pgtol

        where pg_i is the ith component of the projected gradient.
      On exit pgtol is unchanged.

    ws, wy, sy, and wt are double precision working arrays used to
      store the following information defining the limited memory
         BFGS matrix:
         ws, of dimension n x m, stores S, the matrix of s-vectors;
         wy, of dimension n x m, stores Y, the matrix of y-vectors;
         sy, of dimension m x m, stores S'Y;
         ss, of dimension m x m, stores S'S;
         yy, of dimension m x m, stores Y'Y;
         wt, of dimension m x m, stores the Cholesky factorization
                                 of (theta*S'S+LD^(-1)L'); see eq.
                                 (2.26) in [3].

    wn is a double precision working array of dimension 2m x 2m
      used to store the LEL^T factorization of the indefinite matrix
                K = [-D -Y'ZZ'Y/theta     L_a'-R_z'  ]
                    [L_a -R_z           theta*S'AA'S ]

      where     E = [-I  0]
                    [ 0  I]

    snd is a double precision working array of dimension 2m x 2m
      used to store the lower triangular part of
                N = [Y' ZZ'Y   L_a'+R_z']
                    [L_a +R_z  S'AA'S   ]

    z(n),r(n),d(n),t(n), xp(n),wa(8*m) are double precision working arrays.
      z  is used at different times to store the Cauchy point and
         the Newton point.
      xp is used to safeguard the projected Newton direction

    sg(m),sgo(m),yg(m),ygo(m) are double precision working arrays.

    index is an integer working array of dimension n.
      In subroutine freev, index is used to store the free and fixed
         variables at the Generalized Cauchy Point (GCP).

    iwhere is an integer working array of dimension n used to record
      the status of the vector x for GCP computation.
      iwhere(i)=0 or -3 if x(i) is free and has bounds,
                1       if x(i) is fixed at l(i), and l(i) .ne. u(i)
                2       if x(i) is fixed at u(i), and u(i) .ne. l(i)
                3       if x(i) is always fixed, i.e.,  u(i)=x(i)=l(i)
               -1       if x(i) is always free, i.e., no bounds on it.

    indx2 is an integer working array of dimension n.
      Within subroutine cauchy, indx2 corresponds to the array iorder.
      In subroutine freev, a list of variables entering and leaving
      the free set is stored in indx2, and it is passed on to
      subroutine formk with this information.

    task is a working string of characters of length 60 indicating
      the current job when entering and leaving this subroutine.

    iprint is an INTEGER variable that must be set by the user.
      It controls the frequency and type of output generated:
       iprint<0    no output is generated;
       iprint=0    print only one line at the last iteration;
       0<iprint<99 print also f and |proj g| every iprint iterations;
       iprint=99   print details of every iteration except n-vectors;
       iprint=100  print also the changes of active set and final x;
       iprint>100  print details of every iteration including x and g;
      When iprint > 0, the file iterate.dat will be created to
                       summarize the iteration.

    csave is a working string of characters of length 60.

    lsave is a logical working array of dimension 4.

    isave is an integer working array of dimension 23.

    dsave is a double precision working array of dimension 29.


    Subprograms called

      L-BFGS-B Library ... cauchy, subsm, lnsrlb, formk,

       errclb, prn1lb, prn2lb, prn3lb, active, projgr,

       freev, cmprlb, matupd, formt.

      Minpack2 Library ... timer

      Linpack Library ... dcopy, ddot.


    References:

      [1] R. H. Byrd, P. Lu, J. Nocedal and C. Zhu, ``A limited
      memory algorithm for bound constrained optimization'',
      SIAM J. Scientific Computing 16 (1995), no. 5, pp. 1190--1208.

      [2] C. Zhu, R.H. Byrd, P. Lu, J. Nocedal, ``L-BFGS-B: FORTRAN
      Subroutines for Large Scale Bound Constrained Optimization''
      Tech. Report, NAM-11, EECS Department, Northwestern University,
      1994.

      [3] R. Byrd, J. Nocedal and R. Schnabel "Representations of
      Quasi-Newton Matrices and their use in Limited Memory Methods'',
      Mathematical Programming 63 (1994), no. 4, pp. 129-156.

      (Postscript files of these papers are available via anonymous
       ftp to eecs.nwu.edu in the directory pub/lbfgs/lbfgs_bcm.)

                          *  *  *

    NEOS, November 1994. (Latest revision June 1996.)
    Optimization Technology Center.
    Argonne National Laboratory and Northwestern University.
    Written by
                       Ciyou Zhu
    in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal.
    */


    /*     ************ */
    /* Parameter adjustments */
    --indx2;
    --iwhere;
    --index;
    --xp;
    --t;
    --d__;
    --r__;
    --z__;
    --g;
    --nbd;
    --u;
    --l;
    --x;
    --wa;
    snd_dim1 = 2 * *m;
    snd_offset = 1 + snd_dim1;
    snd -= snd_offset;
    wn_dim1 = 2 * *m;
    wn_offset = 1 + wn_dim1;
    wn -= wn_offset;
    wt_dim1 = *m;
    wt_offset = 1 + wt_dim1;
    wt -= wt_offset;
    ss_dim1 = *m;
    ss_offset = 1 + ss_dim1;
    ss -= ss_offset;
    sy_dim1 = *m;
    sy_offset = 1 + sy_dim1;
    sy -= sy_offset;
    wy_dim1 = *n;
    wy_offset = 1 + wy_dim1;
    wy -= wy_offset;
    ws_dim1 = *n;
    ws_offset = 1 + ws_dim1;
    ws -= ws_offset;
    --lsave;
    --isave;
    --dsave;

    /* Function Body */
    if ( *task == START ) {
        epsmch = DBL_EPSILON;
        timer(&time1);
        /*        Initialize counters and scalars when task='START'. */
        /*           for the limited memory BFGS matrices: */
        col = 0;
        head = 1;
        theta = 1.;
        iupdat = 0;
        updatd = FALSE_;
        iback = 0;
        itail = 0;
        iword = 0;
        nact = 0;
        ileave = 0;
        nenter = 0;
        fold = 0.;
        dnorm = 0.;
        cpu1 = 0.;
        gd = 0.;
        stpmx = 0.;
        sbgnrm = 0.;
        stp = 0.;
        gdold = 0.;
        dtd = 0.;
        /*           for operation counts: */
        iter = 0;
        nfgv = 0;
        nseg = 0;
        nintol = 0;
        nskip = 0;
        nfree = *n;
        ifun = 0;
        /*           for stopping tolerance: */
        tol = *factr * epsmch;
        /*           for measuring running time: */
        cachyt = 0.;
        sbtime = 0.;
        lnscht = 0.;
        /*           'word' records the status of subspace solutions. */
        *word = WORD_DEFAULT;
        /*         s_copy(word, "---", (ftnlen)3, (ftnlen)3); */
        /*           'info' records the termination information. */
        info = 0;
        itfile = 8;
        /* Note: no longer trying to write to file */
        /*        Check the input arguments for errors. */
        errclb(n, m, factr, &l[1], &u[1], &nbd[1], task, &info, &k, (ftnlen)
               60);
        if ( IS_ERROR(*task) ){
            prn3lb(n, &x[1], f, task, iprint, &info, o__1, &iter, &nfgv, &
                                                                         nintol, &nskip, &nact, &sbgnrm, &c_b7, &nseg, word, &
                   iback, &stp, &xstep, &k, &cachyt, &sbtime, &lnscht, (
                       ftnlen)60, (ftnlen)3);
            return 0;
        }
        prn1lb(n, m, &l[1], &u[1], &x[1], iprint, o__1, &epsmch);
        active(n, &l[1], &u[1], &nbd[1], &x[1], &iwhere[1], iprint, &prjctd,
               &cnstnd, &boxed);
        /*        The end of the initialization. */
    } else {
        /*          restore local variables. */
        prjctd = lsave[1];
        cnstnd = lsave[2];
        boxed = lsave[3];
        updatd = lsave[4];
        nintol = isave[1];
        itfile = isave[3];
        iback = isave[4];
        nskip = isave[5];
        head = isave[6];
        col = isave[7];
        itail = isave[8];
        iter = isave[9];
        iupdat = isave[10];
        nseg = isave[12];
        nfgv = isave[13];
        info = isave[14];
        ifun = isave[15];
        iword = isave[16];
        nfree = isave[17];
        nact = isave[18];
        ileave = isave[19];
        nenter = isave[20];
        theta = dsave[1];
        fold = dsave[2];
        tol = dsave[3];
        dnorm = dsave[4];
        epsmch = dsave[5];
        cpu1 = dsave[6];
        cachyt = dsave[7];
        sbtime = dsave[8];
        lnscht = dsave[9];
        time1 = dsave[10];
        gd = dsave[11];
        stpmx = dsave[12];
        sbgnrm = dsave[13];
        stp = dsave[14];
        gdold = dsave[15];
        dtd = dsave[16];
        /*        After returning from the driver go to the point where execution */
        /*        is to resume. */
        if ( *task == FG_LN ) {
            goto L666;
        }
        if ( *task == NEW_X ) {
            goto L777;
        }
        if ( *task == FG_ST ) {
            goto L111;
        }
        if ( IS_STOP(*task) ) {
            if ( *task == STOP_CPU ) {
                /*    restore the previous iterate. */
                dcopy(n, &t[1], &c__1, &x[1], &c__1);
                dcopy(n, &r__[1], &c__1, &g[1], &c__1);
                *f = fold;
            }
            goto L999;
        }
    }
    /*     Compute f0 and g0. */
    *task = FG_START;
    /*    return to the driver to calculate f and g; reenter at 111. */
    goto L1000;
L111:
    nfgv = 1;
    /*     Compute the infinity norm of the (-) projected gradient. */
    projgr(n, &l[1], &u[1], &nbd[1], &x[1], &g[1], &sbgnrm);
    if (*iprint >= 1) {
        printf("At iterate %5ld, f(x)= %5.2e, ||proj grad||_infty = %.2e\n",iter,*f,sbgnrm );
    }
    if (sbgnrm <= *pgtol) {
        /*    terminate the algorithm. */
        *task = CONV_GRAD;
        goto L999;
    }
    /* ----------------- the beginning of the loop -------------------------- */
L222:
    if (*iprint >= 99) {
        printf("ITERATION %5ld\n", i__1 );
    }
    iword = -1;

    if (! cnstnd && col > 0) {
        /*    skip the search for GCP. */
        dcopy(n, &x[1], &c__1, &z__[1], &c__1);
        wrk = updatd;
        nseg = 0;
        goto L333;
    }
    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */

    /*     Compute the Generalized Cauchy Point (GCP). */

    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */
    timer(&cpu1);
    info = 0;
    cauchy(n, &x[1], &l[1], &u[1], &nbd[1], &g[1], &indx2[1], &iwhere[1], &t[
                                                                              1], &d__[1], &z__[1], m, &wy[wy_offset], &ws[ws_offset], &sy[
               sy_offset], &wt[wt_offset], &theta, &col, &head, &wa[1], &wa[(*m
                << 1) + 1], &wa[(*m << 2) + 1], &wa[*m * 6 + 1], &nseg, iprint, &
           sbgnrm, &info, &epsmch);
    if (info != 0) {
        /*         singular triangular system detected; refresh the lbfgs memory. */
        info = 0;
        col = 0;
        head = 1;
        theta = 1.;
        iupdat = 0;
        updatd = FALSE_;
        timer(&cpu2);
        cachyt = cachyt + cpu2 - cpu1;
        goto L222;
    }
    timer(&cpu2);
    cachyt = cachyt + cpu2 - cpu1;
    nintol += nseg;
    /*     Count the entering and leaving variables for iter > 0; */
    /*     find the index set of free and active variables at the GCP. */
    freev(n, &nfree, &index[1], &nenter, &ileave, &indx2[1], &iwhere[1], &
                                                                         wrk, &updatd, &cnstnd, iprint, &iter);
    nact = *n - nfree;
L333:
    /*     If there are no free variables or B=theta*I, then */
    /*                                        skip the subspace minimization. */
    if (nfree == 0 || col == 0) {
        goto L555;
    }
    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */

    /*     Subspace minimization. */

    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */
    timer(&cpu1);
    /*     Form  the LEL^T factorization of the indefinite */
    /*       matrix    K = [-D -Y'ZZ'Y/theta     L_a'-R_z'  ] */
    /*                     [L_a -R_z           theta*S'AA'S ] */
    /*       where     E = [-I  0] */
    /*                     [ 0  I] */
    if (wrk) {
        formk(n, &nfree, &index[1], &nenter, &ileave, &indx2[1], &iupdat, &
                                                                          updatd, &wn[wn_offset], &snd[snd_offset], m, &ws[ws_offset], &
              wy[wy_offset], &sy[sy_offset], &theta, &col, &head, &info);
    }
    if (info != 0) {
        /*          nonpositive definiteness in Cholesky factorization; */
        /*          refresh the lbfgs memory and restart the iteration. */
        if (*iprint >= 1) {
            printf(" Nonpositive definiteness in Cholesky factorization in formk;\n");
            printf("   refresh the lbfgs memory and restart the iteration.\n");
        }
        info = 0;
        col = 0;
        head = 1;
        theta = 1.;
        iupdat = 0;
        updatd = FALSE_;
        timer(&cpu2);
        sbtime = sbtime + cpu2 - cpu1;
        goto L222;
    }
    /*        compute r=-Z'B(xcp-xk)-Z'g (using wa(2m+1)=W'(xcp-x) */
    /*                                                   from 'cauchy'). */
    cmprlb(n, m, &x[1], &g[1], &ws[ws_offset], &wy[wy_offset], &sy[sy_offset]
           , &wt[wt_offset], &z__[1], &r__[1], &wa[1], &index[1], &theta, &
           col, &head, &nfree, &cnstnd, &info);
    if (info != 0) {
        goto L444;
    }
    /* -jlm-jn   call the direct method. */
    subsm(n, m, &nfree, &index[1], &l[1], &u[1], &nbd[1], &z__[1], &r__[1], &
                                                                            xp[1], &ws[ws_offset], &wy[wy_offset], &theta, &x[1], &g[1], &col,
          &head, &iword, &wa[1], &wn[wn_offset], iprint, &info);
L444:
    if (info != 0) {
        /*          singular triangular system detected; */
        /*          refresh the lbfgs memory and restart the iteration. */
        if (*iprint >= 1) {
            printf(" Singular triangular system detected;\n");
            printf("   refresh the lbfgs memory and restart the iteration.\n");
        }
        info = 0;
        col = 0;
        head = 1;
        theta = 1.;
        iupdat = 0;
        updatd = FALSE_;
        timer(&cpu2);
        sbtime = sbtime + cpu2 - cpu1;
        goto L222;
    }
    timer(&cpu2);
    sbtime = sbtime + cpu2 - cpu1;
L555:
    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */

    /*     Line search and optimality tests. */

    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */
    /*     Generate the search direction d:=z-x. */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
        d__[i__] = z__[i__] - x[i__];
        /* L40: */
    }
    timer(&cpu1);
L666:
    lnsrlb(n, &l[1], &u[1], &nbd[1], &x[1], f, &fold, &gd, &gdold, &g[1], &
                                                                          d__[1], &r__[1], &t[1], &z__[1], &stp, &dnorm, &dtd, &xstep, &
           stpmx, &iter, &ifun, &iback, &nfgv, &info, task, &boxed, &cnstnd,
           csave, &isave[22], &dsave[17]); /* (ftnlen)60, (ftnlen)60); */
    if (info != 0 || iback >= 20) {
        /*          restore the previous iterate. */
        dcopy(n, &t[1], &c__1, &x[1], &c__1);
        dcopy(n, &r__[1], &c__1, &g[1], &c__1);
        *f = fold;
        if (col == 0) {
            /*             abnormal termination. */
            if (info == 0) {
                info = -9;
                /*                restore the actual number of f and g evaluations etc. */
                --nfgv;
                --ifun;
                --iback;
            }
            *task = ABNORMAL;
            ++iter;
            goto L999;
        } else {
            /*             refresh the lbfgs memory and restart the iteration. */
            if (*iprint >= 1) {
                printf(" Bad direction in the line search;\n");
                printf("   refresh the lbfgs memory and restart the iteration.\n");
            }
            if (info == 0) {
                --nfgv;
            }
            info = 0;
            col = 0;
            head = 1;
            theta = 1.;
            iupdat = 0;
            updatd = FALSE_;
            *task = RESTART;
            timer(&cpu2);
            lnscht = lnscht + cpu2 - cpu1;
            goto L222;
        }
    } else if ( *task == FG_LN ) {
        /*          return to the driver for calculating f and g; reenter at 666. */
        goto L1000;
    } else {
        /*          calculate and print out the quantities related to the new X. */
        timer(&cpu2);
        lnscht = lnscht + cpu2 - cpu1;
        ++iter;
        /*        Compute the infinity norm of the projected (-)gradient. */
        projgr(n, &l[1], &u[1], &nbd[1], &x[1], &g[1], &sbgnrm);
        /*        Print iteration information. */



        prn2lb(n, &x[1], f, &g[1], iprint, o__1, &iter, &nfgv, &nact, &
                                                                      sbgnrm, &nseg, word, &iword, &iback, &stp, &xstep, (ftnlen)3);
        goto L1000;
    }
L777:
    /*     Test for termination. */
    if (sbgnrm <= *pgtol) {
        /*                                terminate the algorithm. */
        *task = CONV_GRAD;
        goto L999;
    }
    /* Computing MAX */
    d__1 = fabs(fold), d__2 = fabs(*f), d__1 = fmax(d__1,d__2);
    ddum = fmax(d__1,1.);
    if (fold - *f <= tol * ddum) {
        /*                                        terminate the algorithm. */
        *task = CONV_F;
        if (iback >= 10) {
            info = -5;
        }
        /*           i.e., to issue a warning if iback>10 in the line search. */
        goto L999;
    }
    /*     Compute d=newx-oldx, r=newg-oldg, rr=y'y and dr=y's. */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
        r__[i__] = g[i__] - r__[i__];
        /* L42: */
    }
    rr = ddot(n, &r__[1], &c__1, &r__[1], &c__1);
    if (stp == 1.) {
        dr = gd - gdold;
        ddum = -gdold;
    } else {
        dr = (gd - gdold) * stp;
        dscal(n, &stp, &d__[1], &c__1);
        ddum = -gdold * stp;
    }
    if (dr <= epsmch * ddum) {
        /*                            skip the L-BFGS update. */
        ++nskip;
        updatd = FALSE_;
        if (*iprint >= 1) {
            printf("  ys=%10.3e  -gs=%10.3e BFGS update SKIPPED\n", dr, ddum );
        }
        goto L888;
    }
    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */

    /*     Update the L-BFGS matrix. */

    /* ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc */
    updatd = TRUE_;
    ++iupdat;
    /*     Update matrices WS and WY and form the middle matrix in B. */
    matupd(n, m, &ws[ws_offset], &wy[wy_offset], &sy[sy_offset], &ss[
                                                                     ss_offset], &d__[1], &r__[1], &itail, &iupdat, &col, &head, &
           theta, &rr, &dr, &stp, &dtd);
    /*     Form the upper half of the pds T = theta*SS + L*D^(-1)*L'; */
    /*        Store T in the upper triangular of the array wt; */
    /*        Cholesky factorize T to J*J' with */
    /*           J' stored in the upper triangular of wt. */
    formt(m, &wt[wt_offset], &sy[sy_offset], &ss[ss_offset], &col, &theta, &
                                                                           info);
    if (info != 0) {
        /*          nonpositive definiteness in Cholesky factorization; */
        /*          refresh the lbfgs memory and restart the iteration. */
        if (*iprint >= 1) {
            printf(" Nonpositive definiteness in Cholesky factorization in formt;\n");
            printf("   refresh the lbfgs memory and restart the iteration.\n");
        }
        info = 0;
        col = 0;
        head = 1;
        theta = 1.;
        iupdat = 0;
        updatd = FALSE_;
        goto L222;
    }
    /*     Now the inverse of the middle matrix in B is */
    /*       [  D^(1/2)      O ] [ -D^(1/2)  D^(-1/2)*L' ] */
    /*       [ -L*D^(-1/2)   J ] [  0        J'          ] */
L888:
    /* -------------------- the end of the loop ----------------------------- */
    goto L222;
L999:
    timer(&time2);
    time = time2 - time1;
    prn3lb(n, &x[1], f, task, iprint, &info, o__1, &iter, &nfgv, &nintol,
           &nskip, &nact, &sbgnrm, &time, &nseg, word, &iback, &stp, &xstep,
           &k, &cachyt, &sbtime, &lnscht, (ftnlen)60, (ftnlen)3);
L1000:
    /*     Save local variables. */
    lsave[1] = prjctd;
    lsave[2] = cnstnd;
    lsave[3] = boxed;
    lsave[4] = updatd;
    isave[1] = nintol;
    isave[3] = itfile;
    isave[4] = iback;
    isave[5] = nskip;
    isave[6] = head;
    isave[7] = col;
    isave[8] = itail;
    isave[9] = iter;
    isave[10] = iupdat;
    isave[12] = nseg;
    isave[13] = nfgv;
    isave[14] = info;
    isave[15] = ifun;
    isave[16] = iword;
    isave[17] = nfree;
    isave[18] = nact;
    isave[19] = ileave;
    isave[20] = nenter;
    dsave[1] = theta;
    dsave[2] = fold;
    dsave[3] = tol;
    dsave[4] = dnorm;
    dsave[5] = epsmch;
    dsave[6] = cpu1;
    dsave[7] = cachyt;
    dsave[8] = sbtime;
    dsave[9] = lnscht;
    dsave[10] = time1;
    dsave[11] = gd;
    dsave[12] = stpmx;
    dsave[13] = sbgnrm;
    dsave[14] = stp;
    dsave[15] = gdold;
    dsave[16] = dtd;
    return 0;
} /* mainlb */




/* Subroutine */ int Lbfgs::setulb(integer *n, integer *m, double *x,
                                   double *l, double *u, integer *nbd, double *f, double
                                       *g, double *factr, double *pgtol, double *wa, integer *
                                       iwa, integer *task, integer *iprint, integer *csave, logical *lsave,
                                   integer *isave, double *dsave) /* ftnlen task_len, ftnlen csave_len) */
{
    /* System generated locals */
    integer i__1;


    /* Local variables */
    integer ld, lr, lt, lz, lwa, lwn, lss, lxp, lws, lwt, lsy, lwy,
        lsnd;

    /* -jlm-jn */
    /*
    ************

    Subroutine setulb

    This subroutine partitions the working arrays wa and iwa, and
      then uses the limited memory BFGS method to solve the bound
      constrained optimization problem by calling mainlb.
      (The direct method will be used in the subspace minimization.)

    n is an integer variable.
      On entry n is the dimension of the problem.
      On exit n is unchanged.

    m is an integer variable.
      On entry m is the maximum number of variable metric corrections
        used to define the limited memory matrix.
      On exit m is unchanged.

    x is a double precision array of dimension n.
      On entry x is an approximation to the solution.
      On exit x is the current approximation.

    l is a double precision array of dimension n.
      On entry l is the lower bound on x.
      On exit l is unchanged.

    u is a double precision array of dimension n.
      On entry u is the upper bound on x.
      On exit u is unchanged.

    nbd is an integer array of dimension n.
      On entry nbd represents the type of bounds imposed on the
        variables, and must be specified as follows:
        nbd(i)=0 if x(i) is unbounded,
               1 if x(i) has only a lower bound,
               2 if x(i) has both lower and upper bounds, and
               3 if x(i) has only an upper bound.
      On exit nbd is unchanged.

    f is a double precision variable.
      On first entry f is unspecified.
      On final exit f is the value of the function at x.

    g is a double precision array of dimension n.
      On first entry g is unspecified.
      On final exit g is the value of the gradient at x.

    factr is a double precision variable.
      On entry factr >= 0 is specified by the user.  The iteration
        will stop when

        (f^k - f^{k+1})/max{|f^k|,|f^{k+1}|,1} <= factr*epsmch

        where epsmch is the machine precision, which is automatically
        generated by the code. Typical values for factr: 1.d+12 for
        low accuracy; 1.d+7 for moderate accuracy; 1.d+1 for extremely
        high accuracy.
      On exit factr is unchanged.

    pgtol is a double precision variable.
      On entry pgtol >= 0 is specified by the user.  The iteration
        will stop when

                max{|proj g_i | i = 1, ..., n} <= pgtol

        where pg_i is the ith component of the projected gradient.
      On exit pgtol is unchanged.

    wa is a double precision working array of length
      (2mmax + 5)nmax + 12mmax^2 + 12mmax.

    iwa is an integer working array of length 3nmax.

    task is a working string of characters of length 60 indicating
      the current job when entering and quitting this subroutine.

    iprint is an integer variable that must be set by the user.
      It controls the frequency and type of output generated:
       iprint<0    no output is generated;
       iprint=0    print only one line at the last iteration;
       0<iprint<99 print also f and |proj g| every iprint iterations;
       iprint=99   print details of every iteration except n-vectors;
       iprint=100  print also the changes of active set and final x;
       iprint>100  print details of every iteration including x and g;
      When iprint > 0, the file iterate.dat will be created to
                       summarize the iteration.

    csave is a working string of characters of length 60.

    lsave is a logical working array of dimension 4.
      On exit with 'task' = NEW_X, the following information is
                                                            available:
        If lsave(1) = .true.  then  the initial X has been replaced by
                                    its projection in the feasible set;
        If lsave(2) = .true.  then  the problem is constrained;
        If lsave(3) = .true.  then  each variable has upper and lower
                                    bounds;

    isave is an integer working array of dimension 44.
      On exit with 'task' = NEW_X, the following information is
                                                            available:
        isave(22) = the total number of intervals explored in the
                        search of Cauchy points;
        isave(26) = the total number of skipped BFGS updates before
                        the current iteration;
        isave(30) = the number of current iteration;
        isave(31) = the total number of BFGS updates prior the current
                        iteration;
        isave(33) = the number of intervals explored in the search of
                        Cauchy point in the current iteration;
        isave(34) = the total number of function and gradient
                        evaluations;
        isave(36) = the number of function value or gradient
                                 evaluations in the current iteration;
        if isave(37) = 0  then the subspace argmin is within the box;
        if isave(37) = 1  then the subspace argmin is beyond the box;
        isave(38) = the number of free variables in the current
                        iteration;
        isave(39) = the number of active constraints in the current
                        iteration;
        n + 1 - isave(40) = the number of variables leaving the set of
                          active constraints in the current iteration;
        isave(41) = the number of variables entering the set of active
                        constraints in the current iteration.

    dsave is a double precision working array of dimension 29.
      On exit with 'task' = NEW_X, the following information is
                                                            available:
        dsave(1) = current 'theta' in the BFGS matrix;
        dsave(2) = f(x) in the previous iteration;
        dsave(3) = factr*epsmch;
        dsave(4) = 2-norm of the line search direction vector;
        dsave(5) = the machine precision epsmch generated by the code;
        dsave(7) = the accumulated time spent on searching for
                                                        Cauchy points;
        dsave(8) = the accumulated time spent on
                                                subspace minimization;
        dsave(9) = the accumulated time spent on line search;
        dsave(11) = the slope of the line search function at
                                 the current point of line search;
        dsave(12) = the maximum relative step length imposed in
                                                          line search;
        dsave(13) = the infinity norm of the projected gradient;
        dsave(14) = the relative step length in the line search;
        dsave(15) = the slope of the line search function at
                                the starting point of the line search;
        dsave(16) = the square of the 2-norm of the line search
                                                     direction vector.

    Subprograms called:

      L-BFGS-B Library ... mainlb.


    References:

      [1] R. H. Byrd, P. Lu, J. Nocedal and C. Zhu, ``A limited
      memory algorithm for bound constrained optimization'',
      SIAM J. Scientific Computing 16 (1995), no. 5, pp. 1190--1208.

      [2] C. Zhu, R.H. Byrd, P. Lu, J. Nocedal, ``L-BFGS-B: a
      limited memory FORTRAN code for solving bound constrained
      optimization problems'', Tech. Report, NAM-11, EECS Department,
      Northwestern University, 1994.

      (Postscript files of these papers are available via anonymous
       ftp to eecs.nwu.edu in the directory pub/lbfgs/lbfgs_bcm.)

                          *  *  *

    NEOS, November 1994. (Latest revision June 1996.)
    Optimization Technology Center.
    Argonne National Laboratory and Northwestern University.
    Written by
                       Ciyou Zhu
    in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal.


    ************
    */
    /* -jlm-jn */
    /* Parameter adjustments */
    --iwa;
    --g;
    --nbd;
    --u;
    --l;
    --x;
    --wa;
    --lsave;
    --isave;
    --dsave;

    /* Function Body */
    if ( *task == START ) {
        isave[1] = *m * *n;
        /* Computing 2nd power */
        i__1 = *m;
        isave[2] = i__1 * i__1;
        /* Computing 2nd power */
        i__1 = *m;
        isave[3] = i__1 * i__1 << 2;
        isave[4] = 1;
        /* ws      m*n */
        isave[5] = isave[4] + isave[1];
        /* wy      m*n */
        isave[6] = isave[5] + isave[1];
        /* wsy     m**2 */
        isave[7] = isave[6] + isave[2];
        /* wss     m**2 */
        isave[8] = isave[7] + isave[2];
        /* wt      m**2 */
        isave[9] = isave[8] + isave[2];
        /* wn      4*m**2 */
        isave[10] = isave[9] + isave[3];
        /* wsnd    4*m**2 */
        isave[11] = isave[10] + isave[3];
        /* wz      n */
        isave[12] = isave[11] + *n;
        /* wr      n */
        isave[13] = isave[12] + *n;
        /* wd      n */
        isave[14] = isave[13] + *n;
        /* wt      n */
        isave[15] = isave[14] + *n;
        /* wxp     n */
        isave[16] = isave[15] + *n;
        /* wa      8*m */
    }
    lws = isave[4];
    lwy = isave[5];
    lsy = isave[6];
    lss = isave[7];
    lwt = isave[8];
    lwn = isave[9];
    lsnd = isave[10];
    lz = isave[11];
    lr = isave[12];
    ld = isave[13];
    lt = isave[14];
    lxp = isave[15];
    lwa = isave[16];
    mainlb(n, m, &x[1], &l[1], &u[1], &nbd[1], f, &g[1], factr, pgtol, &wa[
                                                                           lws], &wa[lwy], &wa[lsy], &wa[lss], &wa[lwt], &wa[lwn], &wa[lsnd],
           &wa[lz], &wa[lr], &wa[ld], &wa[lt], &wa[lxp], &wa[lwa], &iwa[1],
           &iwa[*n + 1], &iwa[(*n << 1) + 1], task, iprint, csave, &lsave[1],
           &isave[22], &dsave[1]); /* (ftnlen)60, (ftnlen)60); */
    return 0;
} /* setulb_ */



int Lbfgs::lnsrlb(integer *n, double *l, double *u,
                  integer *nbd, double *x, double *f, double *fold,
                  double *gd, double *gdold, double *g, double *d__,
                  double *r__, double *t, double *z__, double *stp,
                  double *dnorm, double *dtd, double *xstep, double *
                      stpmx, integer *iter, integer *ifun, integer *iback, integer *nfgv,
                  integer *info, integer *task, logical *boxed, logical *cnstnd, integer *
                      csave, integer *isave, double *dsave) /* ftnlen task_len, ftnlen
   csave_len) */
{
    /*
   **********

   Subroutine lnsrlb

   This subroutine calls subroutine dcsrch from the Minpack2 library
     to perform the line search.  Subroutine dscrch is safeguarded so
     that all trial points lie within the feasible region.

   Subprograms called:

     Minpack2 Library ... dcsrch.

     Linpack ... dtrsl, ddot.


                         *  *  *

   NEOS, November 1994. (Latest revision June 1996.)
   Optimization Technology Center.
   Argonne National Laboratory and Northwestern University.
   Written by
                      Ciyou Zhu
   in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal.


   **********
   */


    /* Table of constant values */
    static double c_b14 = FTOL;
    static double c_b15 = GTOL;
    static double c_b16 = XTOL;
    static double c_b17 = STEPMIN;
    /* System generated locals */
    integer i__1;
    double d__1;


    /* Local variables */
    integer i__;
    double a1, a2;

    /* Parameter adjustments */
    --z__;
    --t;
    --r__;
    --d__;
    --g;
    --x;
    --nbd;
    --u;
    --l;
    --isave;
    --dsave;

    /* Function Body */
    if ( *task == FG_LN ) {
        goto L556;
    }
    *dtd = ddot(n, &d__[1], &c__1, &d__[1], &c__1);
    *dnorm = sqrt(*dtd);
    /*     Determine the maximum step length. */
    *stpmx = 1e10;
    if (*cnstnd) {
        if (*iter == 0) {
            *stpmx = 1.;
        } else {
            i__1 = *n;
            for (i__ = 1; i__ <= i__1; ++i__) {
                a1 = d__[i__];
                if (nbd[i__] != 0) {
                    if (a1 < 0. && nbd[i__] <= 2) {
                        a2 = l[i__] - x[i__];
                        if (a2 >= 0.) {
                            *stpmx = 0.;
                        } else if (a1 * *stpmx < a2) {
                            *stpmx = a2 / a1;
                        }
                    } else if (a1 > 0. && nbd[i__] >= 2) {
                        a2 = u[i__] - x[i__];
                        if (a2 <= 0.) {
                            *stpmx = 0.;
                        } else if (a1 * *stpmx > a2) {
                            *stpmx = a2 / a1;
                        }
                    }
                }
                /* L43: */
            }
        }
    }
    if (*iter == 0 && ! (*boxed)) {
        /* Computing MIN */
        d__1 = 1. / *dnorm;
        *stp = fmin(d__1,*stpmx);
    } else {
        *stp = 1.;
    }
    dcopy(n, &x[1], &c__1, &t[1], &c__1);
    dcopy(n, &g[1], &c__1, &r__[1], &c__1);
    *fold = *f;
    *ifun = 0;
    *iback = 0;
    *csave = START;
L556:
    *gd = ddot(n, &g[1], &c__1, &d__[1], &c__1);
    if (*ifun == 0) {
        *gdold = *gd;
        if (*gd >= 0.) {
            /*  the directional derivative >=0. */
            /*  Line search is impossible. */
            *info = -4;
            return 0;
        }
    }
    dcsrch(f, gd, stp, &c_b14, &c_b15, &c_b16, &c_b17, stpmx, csave, &isave[
                                                                         1], &dsave[1]);/* (ftnlen)60);*/
    *xstep = *stp * *dnorm;
    if (  !(IS_WARNING(*csave)) && !(IS_CONVERGED(*csave)) )  {
        /*     if (     !( (csave>=WARNING)&&(csave<=WARNING_END) )   &&   */
        /*              !( (csave>=CONVERGENCE)&&(csave<=CONVERGENCE_END) )   ) { */
        /*     if (s_cmp(csave, "CONV", (ftnlen)4, (ftnlen)4) != 0 && s_cmp(csave, "WARN" */
        /*                 , (ftnlen)4, (ftnlen)4) != 0) { */
        *task = FG_LNSRCH;
        ++(*ifun);
        ++(*nfgv);
        *iback = *ifun - 1;
        if (*stp == 1.) {
            dcopy(n, &z__[1], &c__1, &x[1], &c__1);
        } else {
            i__1 = *n;
            for (i__ = 1; i__ <= i__1; ++i__) {
                x[i__] = *stp * d__[i__] + t[i__];
                /* L41: */
            }
        }
    } else {
        *task = NEW_X;
    }
    return 0;
} /* lnsrlb */


int Lbfgs::dcsrch(double *f, double *g, double *stp,
                  double *ftol, double *gtol, double *xtol, double *
                      stpmin, double *stpmax, integer *task, integer *isave, double *
                      dsave) /* ftnlen task_len) */
{
    /* System generated locals */
    double d__1;


    /* Local variables */
    double fm, gm, fx, fy, gx, gy, fxm, fym, gxm, gym, stx, sty;
    integer stage;
    double finit, ginit, width, ftest, gtest, stmin, stmax, width1;
    logical brackt;

    /*
     **********

     Subroutine dcsrch

     This subroutine finds a step that satisfies a sufficient
     decrease condition and a curvature condition.

     Each call of the subroutine updates an interval with
     endpoints stx and sty. The interval is initially chosen
     so that it contains a minimizer of the modified function

     psi(stp) = f(stp) - f(0) - ftol*stp*f'(0).

     If psi(stp) <= 0 and f'(stp) >= 0 for some step, then the
     interval is chosen so that it contains a minimizer of f.

     The algorithm is designed to find a step that satisfies
     the sufficient decrease condition

     f(stp) <= f(0) + ftol*stp*f'(0),

     and the curvature condition

     abs(f'(stp)) <= gtol*abs(f'(0)).

     If ftol is less than gtol and if, for example, the function
     is bounded below, then there is always a step which satisfies
     both conditions.

     If no step can be found that satisfies both conditions, then
     the algorithm stops with a warning. In this case stp only
     satisfies the sufficient decrease condition.

     A typical invocation of dcsrch has the following outline:

     task = 'START'
     10 continue
     call dcsrch( ... )
     if (task .eq. 'FG') then
     Evaluate the function and the gradient at stp
     goto 10
     end if

NOTE: The user must no alter work arrays between calls.

The subroutine statement is

subroutine dcsrch(f,g,stp,ftol,gtol,xtol,stpmin,stpmax,
task,isave,dsave)
where

f is a double precision variable.
On initial entry f is the value of the function at 0.
On subsequent entries f is the value of the
function at stp.
On exit f is the value of the function at stp.

g is a double precision variable.
On initial entry g is the derivative of the function at 0.
On subsequent entries g is the derivative of the
function at stp.
On exit g is the derivative of the function at stp.

stp is a double precision variable.
On entry stp is the current estimate of a satisfactory
step. On initial entry, a positive initial estimate
must be provided.
On exit stp is the current estimate of a satisfactory step
if task = 'FG'. If task = 'CONV' then stp satisfies
the sufficient decrease and curvature condition.

    ftol is a double precision variable.
        On entry ftol specifies a nonnegative tolerance for the
        sufficient decrease condition.
        On exit ftol is unchanged.

        gtol is a double precision variable.
        On entry gtol specifies a nonnegative tolerance for the
        curvature condition.
        On exit gtol is unchanged.

        xtol is a double precision variable.
        On entry xtol specifies a nonnegative relative tolerance
        for an acceptable step. The subroutine exits with a
            warning if the relative difference between sty and stx
                is less than xtol.
                On exit xtol is unchanged.

                stpmin is a double precision variable.
                On entry stpmin is a nonnegative lower bound for the step.
                On exit stpmin is unchanged.

                stpmax is a double precision variable.
                On entry stpmax is a nonnegative upper bound for the step.
                On exit stpmax is unchanged.

                task is a character variable of length at least 60.
                On initial entry task must be set to 'START'.
                On exit task indicates the required action:

                If task(1:2) = 'FG' then evaluate the function and
                derivative at stp and call dcsrch again.

                If task(1:4) = 'CONV' then the search is successful.

                If task(1:4) = 'WARN' then the subroutine is not able
                to satisfy the convergence conditions. The exit value of
                stp contains the best point found during the search.

                If task(1:5) = 'ERROR' then there is an error in the
                input arguments.

                On exit with convergence, a warning or an error, the
                variable task contains additional information.

                isave is an integer work array of dimension 2.

                dsave is a double precision work array of dimension 13.

                Subprograms called

                MINPACK-2 ... dcstep

                MINPACK-1 Project. June 1983.
                Argonne National Laboratory.
                Jorge J. More' and David J. Thuente.

                MINPACK-2 Project. October 1993.
                Argonne National Laboratory and University of Minnesota.
                Brett M. Averick, Richard G. Carter, and Jorge J. More'.

                **********
                */
    /*     Initialization block. */
    /* Parameter adjustments */
    --dsave;
    --isave;

    /* Function Body */
    if ( *task == START ) {
        /*        Check the input arguments for errors.  See lbfgsb.h for messages */
        if (*stp < *stpmin)  *task=ERROR_SMALLSTP;
        if (*stp > *stpmax)  *task=ERROR_LARGESTP;
        if (*g >= 0.)        *task=ERROR_INITIAL;
        if (*ftol < 0.)      *task=ERROR_FTOL;
        if (*gtol < 0.)      *task=ERROR_GTOL;
        if (*xtol < 0.)      *task=ERROR_XTOL;
        if (*stpmin < 0.)    *task=ERROR_STP0;
        if (*stpmax < *stpmin) *task=ERROR_STP1;
        /*        Exit if there are errors on input. */
        if ( IS_ERROR(*task) ) {
            return 0;
        }
        /*        Initialize local variables. */
        brackt = FALSE_;
        stage = 1;
        finit = *f;
        ginit = *g;
        gtest = *ftol * ginit;
        width = *stpmax - *stpmin;
        width1 = width / .5;
        /*        The variables stx, fx, gx contain the values of the step, */
        /*        function, and derivative at the best step. */
        /*        The variables sty, fy, gy contain the value of the step, */
        /*        function, and derivative at sty. */
        /*        The variables stp, f, g contain the values of the step, */
        /*        function, and derivative at stp. */
        stx = 0.;
        fx = finit;
        gx = ginit;
        sty = 0.;
        fy = finit;
        gy = ginit;
        stmin = 0.;
        stmax = *stp + *stp * 4.;
        /*         s_copy(task, "FG", task_len, (ftnlen)2); */
        *task = FG;
        goto L1000;
    } else {
        /*        Restore local variables. */
        if (isave[1] == 1) {
            brackt = TRUE_;
        } else {
            brackt = FALSE_;
        }
        stage = isave[2];
        ginit = dsave[1];
        gtest = dsave[2];
        gx = dsave[3];
        gy = dsave[4];
        finit = dsave[5];
        fx = dsave[6];
        fy = dsave[7];
        stx = dsave[8];
        sty = dsave[9];
        stmin = dsave[10];
        stmax = dsave[11];
        width = dsave[12];
        width1 = dsave[13];
    }
    /*     If psi(stp) <= 0 and f'(stp) >= 0 for some step, then the */
    /*     algorithm enters the second stage. */
    ftest = finit + *stp * gtest;
    if (stage == 1 && *f <= ftest && *g >= 0.) {
        stage = 2;
    }
    /*     Test for warnings. */
    if (brackt && (*stp <= stmin || *stp >= stmax)) {
        *task = WARNING_ROUND;
    }
    if (brackt && stmax - stmin <= *xtol * stmax) {
        *task = WARNING_XTOL;
    }
    if (*stp == *stpmax && *f <= ftest && *g <= gtest) {
        *task = WARNING_STPMAX;
    }
    if (*stp == *stpmin && (*f > ftest || *g >= gtest)) {
        *task = WARNING_STPMIN;
    }
    /*     Test for convergence. */
    if (*f <= ftest && fabs(*g) <= *gtol * (-ginit)) {
        *task = CONVERGENCE;
    }
    /*     Test for termination. */
    /*     if (     ( (task>=WARNING)&&(task<=WARNING_END) )    || */
    /*              ( (task>=CONVERGENCE)&&(task<=CONVERGENCE_END) )   ) { */
    if ( (IS_WARNING(*task)) || (IS_CONVERGED(*task) ) ) {
        goto L1000;
    }
    /*     A modified function is used to predict the step during the */
    /*     first stage if a lower function value has been obtained but */
    /*     the decrease is not sufficient. */
    if (stage == 1 && *f <= fx && *f > ftest) {
        /*        Define the modified function and derivative values. */
        fm = *f - *stp * gtest;
        fxm = fx - stx * gtest;
        fym = fy - sty * gtest;
        gm = *g - gtest;
        gxm = gx - gtest;
        gym = gy - gtest;
        /*        Call dcstep to update stx, sty, and to compute the new step. */
        dcstep(&stx, &fxm, &gxm, &sty, &fym, &gym, stp, &fm, &gm, &brackt, &
                                                                           stmin, &stmax);
        /*        Reset the function and derivative values for f. */
        fx = fxm + stx * gtest;
        fy = fym + sty * gtest;
        gx = gxm + gtest;
        gy = gym + gtest;
    } else {
        /*       Call dcstep to update stx, sty, and to compute the new step. */
        dcstep(&stx, &fx, &gx, &sty, &fy, &gy, stp, f, g, &brackt, &stmin, &
                                                                           stmax);
    }
    /*     Decide if a bisection step is needed. */
    if (brackt) {
        if ((d__1 = sty - stx, fabs(d__1)) >= width1 * .66) {
            *stp = stx + (sty - stx) * .5;
        }
        width1 = width;
        width = (d__1 = sty - stx, fabs(d__1));
    }
    /*     Set the minimum and maximum steps allowed for stp. */
    if (brackt) {
        stmin = fmin(stx,sty);
        stmax = fmax(stx,sty);
    } else {
        stmin = *stp + (*stp - stx) * 1.1;
        stmax = *stp + (*stp - stx) * 4.;
    }
    /*     Force the step to be within the bounds stpmax and stpmin. */
    *stp = fmax(*stp,*stpmin);
    *stp = fmin(*stp,*stpmax);
    /*     If further progress is not possible, let stp be the best */
    /*     point obtained during the search. */
    /*     if (brackt && (*stp <= stmin || *stp >= stmax) || brackt && stmax - stmin  */
    /* SRB: guess the precedence. && precedence over || */
    if ( (brackt && (*stp <= stmin || *stp >= stmax) ) || (brackt && stmax - stmin
                                                                       <= *xtol * stmax)) {
        *stp = stx;
    }
    /*     Obtain another function and derivative. */
    /*     s_copy(task, "FG", task_len, (ftnlen)2); */
    *task = FG;
L1000:
    /*     Save local variables. */
    if (brackt) {
        isave[1] = 1;
    } else {
        isave[1] = 0;
    }
    isave[2] = stage;
    dsave[1] = ginit;
    dsave[2] = gtest;
    dsave[3] = gx;
    dsave[4] = gy;
    dsave[5] = finit;
    dsave[6] = fx;
    dsave[7] = fy;
    dsave[8] = stx;
    dsave[9] = sty;
    dsave[10] = stmin;
    dsave[11] = stmax;
    dsave[12] = width;
    dsave[13] = width1;
    return 0;
} /* dcsrch */


int Lbfgs::dcstep(double *stx, double *fx, double *dx,
                  double *sty, double *fy, double *dy, double *stp,
                  double *fp, double *dp, logical *brackt, double *stpmin,
                  double *stpmax)
{
    /* System generated locals */
    double d__1, d__2, d__3;



    /* Local variables */
    double p, q, r__, s, sgnd, stpc, stpf, stpq, gamma, theta;

    /*
    **********

    Subroutine dcstep

    This subroutine computes a safeguarded step for a search
    procedure and updates an interval that contains a step that
    satisfies a sufficient decrease and a curvature condition.

    The parameter stx contains the step with the least function
    value. If brackt is set to .true. then a minimizer has
    been bracketed in an interval with endpoints stx and sty.
    The parameter stp contains the current step.
    The subroutine assumes that if brackt is set to .true. then

    min(stx,sty) < stp < fmax(stx,sty),

    and that the derivative at stx is negative in the direction
    of the step.

    The subroutine statement is

    subroutine dcstep(stx,fx,dx,sty,fy,dy,stp,fp,dp,brackt,
    stpmin,stpmax)

    where

    stx is a double precision variable.
    On entry stx is the best step obtained so far and is an
    endpoint of the interval that contains the minimizer.
    On exit stx is the updated best step.

    fx is a double precision variable.
    On entry fx is the function at stx.
    On exit fx is the function at stx.

    dx is a double precision variable.
    On entry dx is the derivative of the function at
    stx. The derivative must be negative in the direction of
    the step, that is, dx and stp - stx must have opposite
    signs.
    On exit dx is the derivative of the function at stx.

    sty is a double precision variable.
    On entry sty is the second endpoint of the interval that
    contains the minimizer.
    On exit sty is the updated endpoint of the interval that
    contains the minimizer.

    fy is a double precision variable.
    On entry fy is the function at sty.
    On exit fy is the function at sty.

    dy is a double precision variable.
    On entry dy is the derivative of the function at sty.
    On exit dy is the derivative of the function at the exit sty.

    stp is a double precision variable.
    On entry stp is the current step. If brackt is set to .true.
    then on input stp must be between stx and sty.
    On exit stp is a new trial step.

    fp is a double precision variable.
    On entry fp is the function at stp
    On exit fp is unchanged.

    dp is a double precision variable.
    On entry dp is the the derivative of the function at stp.
    On exit dp is unchanged.

    brackt is an logical variable.
   On entry brackt specifies if a minimizer has been bracketed.
       Initially brackt must be set to .false.
       On exit brackt specifies if a minimizer has been bracketed.
       When a minimizer is bracketed brackt is set to .true.

       stpmin is a double precision variable.
       On entry stpmin is a lower bound for the step.
       On exit stpmin is unchanged.

       stpmax is a double precision variable.
       On entry stpmax is an upper bound for the step.
       On exit stpmax is unchanged.

       MINPACK-1 Project. June 1983
       Argonne National Laboratory.
       Jorge J. More' and David J. Thuente.

       MINPACK-2 Project. October 1993.
       Argonne National Laboratory and University of Minnesota.
       Brett M. Averick and Jorge J. More'.

       **********
       */
    sgnd = *dp * (*dx / fabs(*dx));
    /*     First case: A higher function value. The minimum is bracketed. */
    /*     If the cubic step is closer to stx than the quadratic step, the */
    /*     cubic step is taken, otherwise the average of the cubic and */
    /*     quadratic steps is taken. */
    if (*fp > *fx) {
        theta = (*fx - *fp) * 3. / (*stp - *stx) + *dx + *dp;
        /* Computing MAX */
        d__1 = fabs(theta), d__2 = fabs(*dx), d__1 = fmax(d__1,d__2),
            d__2 = fabs(*dp);
        s = fmax(d__1,d__2);
        /* Computing 2nd power */
        d__1 = theta / s;
        gamma = s * sqrt(d__1 * d__1 - *dx / s * (*dp / s));
        if (*stp < *stx) {
            gamma = -gamma;
        }
        p = gamma - *dx + theta;
        q = gamma - *dx + gamma + *dp;
        r__ = p / q;
        stpc = *stx + r__ * (*stp - *stx);
        stpq = *stx + *dx / ((*fx - *fp) / (*stp - *stx) + *dx) / 2. * (*stp
                                                                        - *stx);
        if ((d__1 = stpc - *stx, fabs(d__1)) < (d__2 = stpq - *stx, fabs(d__2)))
        {
            stpf = stpc;
        } else {
            stpf = stpc + (stpq - stpc) / 2.;
        }
        *brackt = TRUE_;
        /*     Second case: A lower function value and derivatives of opposite */
        /*     sign. The minimum is bracketed. If the cubic step is farther from */
        /*     stp than the secant step, the cubic step is taken, otherwise the */
        /*     secant step is taken. */
    } else if (sgnd < 0.) {
        theta = (*fx - *fp) * 3. / (*stp - *stx) + *dx + *dp;
        /* Computing MAX */
        d__1 = fabs(theta), d__2 = fabs(*dx), d__1 = fmax(d__1,d__2),
            d__2 = fabs(*dp);
        s = fmax(d__1,d__2);
        /* Computing 2nd power */
        d__1 = theta / s;
        gamma = s * sqrt(d__1 * d__1 - *dx / s * (*dp / s));
        if (*stp > *stx) {
            gamma = -gamma;
        }
        p = gamma - *dp + theta;
        q = gamma - *dp + gamma + *dx;
        r__ = p / q;
        stpc = *stp + r__ * (*stx - *stp);
        stpq = *stp + *dp / (*dp - *dx) * (*stx - *stp);
        if ((d__1 = stpc - *stp, fabs(d__1)) > (d__2 = stpq - *stp, fabs(d__2)))
        {
            stpf = stpc;
        } else {
            stpf = stpq;
        }
        *brackt = TRUE_;
        /*     Third case: A lower function value, derivatives of the same sign, */
        /*     and the magnitude of the derivative decreases. */
    } else if (fabs(*dp) < fabs(*dx)) {
        /*        The cubic step is computed only if the cubic tends to infinity */
        /*        in the direction of the step or if the minimum of the cubic */
        /*        is beyond stp. Otherwise the cubic step is defined to be the */
        /*        secant step. */
        theta = (*fx - *fp) * 3. / (*stp - *stx) + *dx + *dp;
        /* Computing MAX */
        d__1 = fabs(theta), d__2 = fabs(*dx), d__1 = fmax(d__1,d__2),
            d__2 = fabs(*dp);
        s = fmax(d__1,d__2);
        /*        The case gamma = 0 only arises if the cubic does not tend */
        /*        to infinity in the direction of the step. */
        /* Computing MAX */
        /* Computing 2nd power */
        d__3 = theta / s;
        d__1 = 0., d__2 = d__3 * d__3 - *dx / s * (*dp / s);
        gamma = s * sqrt((fmax(d__1,d__2)));
        if (*stp > *stx) {
            gamma = -gamma;
        }
        p = gamma - *dp + theta;
        q = gamma + (*dx - *dp) + gamma;
        r__ = p / q;
        if (r__ < 0. && gamma != 0.) {
            stpc = *stp + r__ * (*stx - *stp);
        } else if (*stp > *stx) {
            stpc = *stpmax;
        } else {
            stpc = *stpmin;
        }
        stpq = *stp + *dp / (*dp - *dx) * (*stx - *stp);
        if (*brackt) {
            /*           A minimizer has been bracketed. If the cubic step is */
            /*           closer to stp than the secant step, the cubic step is */
            /*           taken, otherwise the secant step is taken. */
            if ((d__1 = stpc - *stp, fabs(d__1)) <
                (d__2 = stpq - *stp, fabs(d__2))) {
                stpf = stpc;
            } else {
                stpf = stpq;
            }
            if (*stp > *stx) {
                /* Computing MIN */
                d__1 = *stp + (*sty - *stp) * .66;
                stpf = fmin(d__1,stpf);
            } else {
                /* Computing MAX */
                d__1 = *stp + (*sty - *stp) * .66;
                stpf = fmax(d__1,stpf);
            }
        } else {
            /*           A minimizer has not been bracketed. If the cubic step is */
            /*           farther from stp than the secant step, the cubic step is */
            /*           taken, otherwise the secant step is taken. */
            if ((d__1 = stpc - *stp, fabs(d__1)) >
                (d__2 = stpq - *stp, fabs(d__2))) {
                stpf = stpc;
            } else {
                stpf = stpq;
            }
            stpf = fmin(*stpmax,stpf);
            stpf = fmax(*stpmin,stpf);
        }
        /*     Fourth case: A lower function value, derivatives of the same sign, */
        /*     and the magnitude of the derivative does not decrease. If the */
        /*     minimum is not bracketed, the step is either stpmin or stpmax, */
        /*     otherwise the cubic step is taken. */
    } else {
        if (*brackt) {
            theta = (*fp - *fy) * 3. / (*sty - *stp) + *dy + *dp;
            /* Computing MAX */
            d__1 = fabs(theta), d__2 = fabs(*dy), d__1 = fmax(d__1,d__2),
                d__2 = fabs(*dp);
            s = fmax(d__1,d__2);
            /* Computing 2nd power */
            d__1 = theta / s;
            gamma = s * sqrt(d__1 * d__1 - *dy / s * (*dp / s));
            if (*stp > *sty) {
                gamma = -gamma;
            }
            p = gamma - *dp + theta;
            q = gamma - *dp + gamma + *dy;
            r__ = p / q;
            stpc = *stp + r__ * (*sty - *stp);
            stpf = stpc;
        } else if (*stp > *stx) {
            stpf = *stpmax;
        } else {
            stpf = *stpmin;
        }
    }
    /*     Update the interval which contains a minimizer. */
    if (*fp > *fx) {
        *sty = *stp;
        *fy = *fp;
        *dy = *dp;
    } else {
        if (sgnd < 0.) {
            *sty = *stx;
            *fy = *fx;
            *dy = *dx;
        }
        *stx = *stp;
        *fx = *fp;
        *dx = *dp;
    }
    /*     Compute the new step. */
    *stp = stpf;
    return 0;
} /* dcstep */




int Lbfgs::dpofa(double *a, integer *lda, integer *n, integer *
                                                          info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;



    /* Local variables */
    integer j, k;
    double s, t;
    integer jm1;

    /*
    dpofa factors a double precision symmetric positive definite
    matrix.

    dpofa is usually called by dpoco, but it can be called
    directly with a saving in time if  rcond  is not needed.
    (time for dpoco) = (1 + 18/n)*(time for dpofa) .

    on entry

       a       double precision(lda, n)
               the symmetric matrix to be factored.  only the
               diagonal and upper triangle are used.

       lda     integer
               the leading dimension of the array  a .

       n       integer
               the order of the matrix  a .

    on return

       a       an upper triangular matrix  r  so that  a = trans(r)*r
               where  trans(r)  is the transpose.
               the strict lower triangle is unaltered.
               if  info .ne. 0 , the factorization is not complete.

       info    integer
               = 0  for normal return.
               = k  signals an error condition.  the leading minor
                    of order  k  is not positive definite.

    linpack.  this version dated 08/14/78 .
    cleve moler, university of new mexico, argonne national lab.

    subroutines and functions

    blas ddot
    fortran sqrt
    */

    /*     internal variables */

    /*     begin block with ...exits to 40 */


    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
        *info = j;
        s = 0.;
        jm1 = j - 1;
        if (jm1 < 1) {
            goto L20;
        }
        i__2 = jm1;
        for (k = 1; k <= i__2; ++k) {
            i__3 = k - 1;
            t = a[k + j * a_dim1] - ddot(&i__3, &a[k * a_dim1 + 1], &c__1, &
                                                                           a[j * a_dim1 + 1], &c__1);
            t /= a[k + k * a_dim1];
            a[k + j * a_dim1] = t;
            s += t * t;
            /* L10: */
        }
    L20:
        s = a[j + j * a_dim1] - s;
        /*     ......exit */
        if (s <= 0.) {
            goto L40;
        }
        a[j + j * a_dim1] = sqrt(s);
        /* L30: */
    }
    *info = 0;
L40:
    return 0;
} /* dpofa */

/* ====================== The end of dpofa =============================== */

int Lbfgs::dtrsl(double *t, integer *ldt, integer *n,
                 double *b, integer *job, integer *info)
{
    /* System generated locals */
    integer t_dim1, t_offset, i__1, i__2;

    /* Local variables */
    integer j, jj, case__;
    double temp;
    /*
    extern double ddot(integer *, double *, integer *, double *,
        integer *);
    extern  int daxpy(integer *, double *, double *,
        integer *, double *, integer *);
    */


    /*     dtrsl solves systems of the form */

    /*                   t * x = b */
    /*     or */
    /*                   trans(t) * x = b */

    /*     where t is a triangular matrix of order n. here trans(t) */
    /*     denotes the transpose of the matrix t. */

    /*     on entry */

    /*         t         double precision(ldt,n) */
    /*                   t contains the matrix of the system. the zero */
    /*                   elements of the matrix are not referenced, and */
    /*                   the corresponding elements of the array can be */
    /*                   used to store other information. */

    /*         ldt       integer */
    /*                   ldt is the leading dimension of the array t. */

    /*         n         integer */
    /*                   n is the order of the system. */

    /*         b         double precision(n). */
    /*                   b contains the right hand side of the system. */

    /*         job       integer */
    /*                   job specifies what kind of system is to be solved. */
    /*                   if job is */

    /*                        00   solve t*x=b, t lower triangular, */
    /*                        01   solve t*x=b, t upper triangular, */
    /*                        10   solve trans(t)*x=b, t lower triangular, */
    /*                        11   solve trans(t)*x=b, t upper triangular. */

    /*     on return */

    /*         b         b contains the solution, if info .eq. 0. */
    /*                   otherwise b is unaltered. */

    /*         info      integer */
    /*                   info contains zero if the system is nonsingular. */
    /*                   otherwise info contains the index of */
    /*                   the first zero diagonal element of t. */

    /*     linpack. this version dated 08/14/78 . */
    /*     g. w. stewart, university of maryland, argonne national lab. */

    /*     subroutines and functions */

    /*     blas daxpy,ddot */
    /*     fortran mod */

    /*     internal variables */


    /*     begin block permitting ...exits to 150 */

    /*        check for zero diagonal elements. */

    /* Parameter adjustments */
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    --b;

    /* Function Body */
    i__1 = *n;

    for (*info = 1; *info <= i__1; ++(*info)) {
        /*     ......exit */
        if (t[*info + *info * t_dim1] == 0.) {
            goto L150;
        }
        /* L10: */
    }
    *info = 0;
    /* SRB: there is a problem with this. Not sure */
    /* It keeps returning with info==2.  Feb 17 2015 */

    /*        determine the task and go to it. */

    case__ = 1;
    if (*job % 10 != 0) {
        case__ = 2;
    }
    if (*job % 100 / 10 != 0) {
        case__ += 2;
    }
    switch (case__) {
    case 1:  goto L20;
    case 2:  goto L50;
    case 3:  goto L80;
    case 4:  goto L110;
    }

    /*        solve t*x=b for t lower triangular */

L20:
    b[1] /= t[t_dim1 + 1];
    if (*n < 2) {
        goto L40;
    }
    i__1 = *n;
    for (j = 2; j <= i__1; ++j) {
        temp = -b[j - 1];
        i__2 = *n - j + 1;
        daxpy(&i__2, &temp, &t[j + (j - 1) * t_dim1], &c__1, &b[j], &c__1);
        b[j] /= t[j + j * t_dim1];
        /* L30: */
    }
L40:
    goto L140;

/*        solve t*x=b for t upper triangular. */

L50:
    b[*n] /= t[*n + *n * t_dim1];
    if (*n < 2) {
        goto L70;
    }
    i__1 = *n;
    for (jj = 2; jj <= i__1; ++jj) {
        j = *n - jj + 1;
        temp = -b[j + 1];
        daxpy(&j, &temp, &t[(j + 1) * t_dim1 + 1], &c__1, &b[1], &c__1);
        b[j] /= t[j + j * t_dim1];
        /* L60: */
    }
L70:
    goto L140;

/*        solve trans(t)*x=b for t lower triangular. */

L80:
    b[*n] /= t[*n + *n * t_dim1];
    if (*n < 2) {
        goto L100;
    }
    i__1 = *n;
    for (jj = 2; jj <= i__1; ++jj) {
        j = *n - jj + 1;
        i__2 = jj - 1;
        b[j] -= ddot(&i__2, &t[j + 1 + j * t_dim1], &c__1, &b[j + 1], &c__1);
        b[j] /= t[j + j * t_dim1];
        /* L90: */
    }
L100:
    goto L140;

/*        solve trans(t)*x=b for t upper triangular. */

L110:
    b[1] /= t[t_dim1 + 1];
    if (*n < 2) {
        goto L130;
    }
    i__1 = *n;
    for (j = 2; j <= i__1; ++j) {
        i__2 = j - 1;
        b[j] -= ddot(&i__2, &t[j * t_dim1 + 1], &c__1, &b[1], &c__1);
        b[j] /= t[j + j * t_dim1];
        /* L120: */
    }
L130:
L140:
L150:
    return 0;
} /* dtrsl */



int Lbfgs::daxpy(integer *n, double *da, double *dx,
                 integer *incx, double *dy, integer *incy)
{

    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i, m, ix, iy, mp1;


/*     constant times a vector plus a vector.
       uses unrolled loops for increments equal to one.
       jack dongarra, linpack, 3/11/78.
       modified 12/3/93, array(1) declarations changed to array(*)



       Parameter adjustments
       Function Body */
#define DY(I) dy[(I)-1]
#define DX(I) dx[(I)-1]


    if (*n <= 0) {
        return 0;
    }
    if (*da == 0.) {
        return 0;
    }
    if (*incx == 1 && *incy == 1) {
        goto L20;
    }

    /*        code for unequal increments or equal increments
              not equal to 1 */

    ix = 1;
    iy = 1;
    if (*incx < 0) {
        ix = (-(*n) + 1) * *incx + 1;
    }
    if (*incy < 0) {
        iy = (-(*n) + 1) * *incy + 1;
    }
    i__1 = *n;
    for (i = 1; i <= *n; ++i) {
        DY(iy) += *da * DX(ix);
        ix += *incx;
        iy += *incy;
        /* L10: */
    }
    return 0;

    /*        code for both increments equal to 1


              clean-up loop */

L20:
    m = *n % 4;
    if (m == 0) {
        goto L40;
    }
    i__1 = m;
    for (i = 1; i <= m; ++i) {
        DY(i) += *da * DX(i);
        /* L30: */
    }
    if (*n < 4) {
        return 0;
    }
L40:
    mp1 = m + 1;
    i__1 = *n;
    for (i = mp1; i <= *n; i += 4) {
        DY(i) += *da * DX(i);
        DY(i + 1) += *da * DX(i + 1);
        DY(i + 2) += *da * DX(i + 2);
        DY(i + 3) += *da * DX(i + 3);
        /* L50: */
    }
    return 0;
} /* daxpyRef */


int Lbfgs::dcopy(integer *n, double *dx, integer *incx,
                 double *dy, integer *incy)
{


    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i, m, ix, iy, mp1;


    /*     copies a vector, x, to a vector, y.
           uses unrolled loops for increments equal to one.
           jack dongarra, linpack, 3/11/78.
           modified 12/3/93, array(1) declarations changed to array(*)



           Parameter adjustments
           Function Body */
#define DY(I) dy[(I)-1]
#define DX(I) dx[(I)-1]


    if (*n <= 0) {
        return 0;
    }
    if (*incx == 1 && *incy == 1) {
        goto L20;
    }

    /*        code for unequal increments or equal increments
              not equal to 1 */

    ix = 1;
    iy = 1;
    if (*incx < 0) {
        ix = (-(*n) + 1) * *incx + 1;
    }
    if (*incy < 0) {
        iy = (-(*n) + 1) * *incy + 1;
    }
    i__1 = *n;
    for (i = 1; i <= *n; ++i) {
        DY(iy) = DX(ix);
        ix += *incx;
        iy += *incy;
        /* L10: */
    }
    return 0;

    /*        code for both increments equal to 1


              clean-up loop */

L20:
    m = *n % 7;
    if (m == 0) {
        goto L40;
    }
    i__1 = m;
    for (i = 1; i <= m; ++i) {
        DY(i) = DX(i);
        /* L30: */
    }
    if (*n < 7) {
        return 0;
    }
L40:
    mp1 = m + 1;
    i__1 = *n;
    for (i = mp1; i <= *n; i += 7) {
        DY(i) = DX(i);
        DY(i + 1) = DX(i + 1);
        DY(i + 2) = DX(i + 2);
        DY(i + 3) = DX(i + 3);
        DY(i + 4) = DX(i + 4);
        DY(i + 5) = DX(i + 5);
        DY(i + 6) = DX(i + 6);
        /* L50: */
    }
    return 0;
} /* dcopyRef */

double Lbfgs::ddot(integer *n, double *dx, integer *incx, double *dy,
                   integer *incy)
{


    /* System generated locals */
    integer i__1;
    double ret_val;

    /* Local variables */
    integer i, m;
    double dtemp;
    integer ix, iy, mp1;


    /*     forms the dot product of two vectors.
           uses unrolled loops for increments equal to one.
           jack dongarra, linpack, 3/11/78.
           modified 12/3/93, array(1) declarations changed to array(*)



           Parameter adjustments
           Function Body */
#define DY(I) dy[(I)-1]
#define DX(I) dx[(I)-1]


    ret_val = 0.;
    dtemp = 0.;
    if (*n <= 0) {
        return ret_val;
    }
    if (*incx == 1 && *incy == 1) {
        goto L20;
    }

    /*        code for unequal increments or equal increments
              not equal to 1 */

    ix = 1;
    iy = 1;
    if (*incx < 0) {
        ix = (-(*n) + 1) * *incx + 1;
    }
    if (*incy < 0) {
        iy = (-(*n) + 1) * *incy + 1;
    }
    i__1 = *n;
    for (i = 1; i <= *n; ++i) {
        dtemp += DX(ix) * DY(iy);
        ix += *incx;
        iy += *incy;
        /* L10: */
    }
    ret_val = dtemp;
    return ret_val;

    /*        code for both increments equal to 1


              clean-up loop */

L20:
    m = *n % 5;
    if (m == 0) {
        goto L40;
    }
    i__1 = m;
    for (i = 1; i <= m; ++i) {
        dtemp += DX(i) * DY(i);
        /* L30: */
    }
    if (*n < 5) {
        goto L60;
    }
L40:
    mp1 = m + 1;
    i__1 = *n;
    for (i = mp1; i <= *n; i += 5) {
        dtemp = dtemp + DX(i) * DY(i) + DX(i + 1) * DY(i + 1) + DX(i + 2) *
                                                                    DY(i + 2) + DX(i + 3) * DY(i + 3) + DX(i + 4) * DY(i + 4);
        /* L50: */
    }
L60:
    ret_val = dtemp;
    return ret_val;
} /* ddotRef */


int Lbfgs::dscal(integer *n, double *da, double *dx,
                 integer *incx)
{


    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer i, m, nincx, mp1;


    /*     scales a vector by a constant.
           uses unrolled loops for increment equal to one.
           jack dongarra, linpack, 3/11/78.
           modified 3/93 to return if incx .le. 0.
           modified 12/3/93, array(1) declarations changed to array(*)



           Parameter adjustments
           Function Body */
#define DX(I) dx[(I)-1]


    if (*n <= 0 || *incx <= 0) {
        return 0;
    }
    if (*incx == 1) {
        goto L20;
    }

    /*        code for increment not equal to 1 */

    nincx = *n * *incx;
    i__1 = nincx;
    i__2 = *incx;
    for (i = 1; *incx < 0 ? i >= nincx : i <= nincx; i += *incx) {
        DX(i) = *da * DX(i);
        /* L10: */
    }
    return 0;

    /*        code for increment equal to 1


              clean-up loop */

L20:
    m = *n % 5;
    if (m == 0) {
        goto L40;
    }
    i__2 = m;
    for (i = 1; i <= m; ++i) {
        DX(i) = *da * DX(i);
        /* L30: */
    }
    if (*n < 5) {
        return 0;
    }
L40:
    mp1 = m + 1;
    i__2 = *n;
    for (i = mp1; i <= *n; i += 5) {
        DX(i) = *da * DX(i);
        DX(i + 1) = *da * DX(i + 1);
        DX(i + 2) = *da * DX(i + 2);
        DX(i + 3) = *da * DX(i + 3);
        DX(i + 4) = *da * DX(i + 4);
        /* L50: */
    }
    return 0;
}


int Lbfgs::prn1lb(integer *n, integer *m, double *l,
                  double *u, double *x, integer *iprint, FILE* itfile,
                  double *epsmch)
{
    /*
    ************

    Subroutine prn1lb

    This subroutine prints the input data, initial point, upper and
      lower bounds of each variable, machine precision, as well as
      the headings of the output.


                          *  *  *

    NEOS, November 1994. (Latest revision June 1996.)
    Optimization Technology Center.
    Argonne National Laboratory and Northwestern University.
    Written by
                       Ciyou Zhu
    in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal.


        ************
    */
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__;

    /* Parameter adjustments */
    --x;
    --u;
    --l;

    /* Function Body */
    if (*iprint >= 1) {
        printf("           * * *\n");
        printf("        RUNNING THE L-BFGS-B CODE\n");
        printf("           * * *\n");
        printf("Machine precision = %.2e\n", *epsmch );
        printf(" N = %10ld\n M = %10ld\n", *n, *m );
        if (*iprint >= 1) {
            if (*iprint > 100) {
                printf("L  =");
                i__1 = *n;
                for (i__ = 1; i__ <= i__1; ++i__) {
                    printf("%.2e ", l[i__] );
                }
                printf("\n");
                printf("X0 =");
                i__1 = *n;
                for (i__ = 1; i__ <= i__1; ++i__) {
                    printf("%.2e ", x[i__] );
                }
                printf("\n");
                printf("U  =");
                i__1 = *n;
                for (i__ = 1; i__ <= i__1; ++i__) {
                    printf("%.2e ", u[i__] );
                }
                printf("\n");
            }
        }
    }
    return 0;
} /* prn1lb */

int Lbfgs::prn2lb(integer *n, double *x, double *f,
                  double *g, integer *iprint, FILE* itfile, integer *iter,
                  integer *nfgv, integer *nact, double *sbgnrm, integer *nseg, integer
                      *word, integer *iword, integer *iback, double *stp, double *
                      xstep, ftnlen word_len)
{
    /*
    ************

    Subroutine prn2lb

    This subroutine prints out new information after a successful
      line search.


                          *  *  *

    NEOS, November 1994. (Latest revision June 1996.)
    Optimization Technology Center.
    Argonne National Laboratory and Northwestern University.
    Written by
                       Ciyou Zhu
    in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal.


    ************
    */
    /*           'word' records the status of subspace solutions. */
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, imod;

    /* Parameter adjustments */
    --g;
    --x;

    /* Function Body */
    if (*iword == 0) {
        *word = WORD_CON;
        /*  the subspace minimization converged. */
        /*         s_copy(word, "con", (ftnlen)3, (ftnlen)3); */
    } else if (*iword == 1) {
        *word = WORD_BND;
        /*  the subspace minimization stopped at a bound. */
        /*         s_copy(word, "bnd", (ftnlen)3, (ftnlen)3); */
    } else if (*iword == 5) {
        *word = WORD_TNT;
        /*  the truncated Newton step has been used. */
        /*         s_copy(word, "TNT", (ftnlen)3, (ftnlen)3); */
    } else {
        *word = WORD_DEFAULT;
        /*         s_copy(word, "---", (ftnlen)3, (ftnlen)3); */
    }
    if (*iprint >= 99) {
        printf("LINE SEARCH %ld times; norm of step = %.2e\n", *iback, *xstep );
        printf("At iterate %5ld, f(x)= %5.2e, ||proj grad||_infty = %.2e\n",*iter,*f,*sbgnrm );
        if (*iprint > 100) {
            printf("X =");
            i__1 = *n;
            for (i__ = 1; i__ <= i__1; ++i__) {
                printf("%.2e ", x[i__] );
            }
            printf("\n");
            printf("G =");
            i__1 = *n;
            for (i__ = 1; i__ <= i__1; ++i__) {
                printf("%.2e ", g[i__] );
            }
            printf("\n");
        }
    } else if (*iprint > 0) {
        imod = *iter % *iprint;
        if (imod == 0) {
            printf("At iterate %5ld, f(x)= %5.2e, ||proj grad||_infty = %.2e\n",*iter,*f,*sbgnrm );
        }
    }
    return 0;
} /* prn2lb */

int Lbfgs::prn3lb(integer *n, double *x, double *f, integer *
                                                        task, integer *iprint, integer *info, FILE* itfile, integer *iter,
                  integer *nfgv, integer *nintol, integer *nskip, integer *nact,
                  double *sbgnrm, double *time, integer *nseg, integer *word,
                  integer *iback, double *stp, double *xstep, integer *k,
                  double *cachyt, double *sbtime, double *lnscht, ftnlen
                      task_len, ftnlen word_len)
{
    /*
    ************

    Subroutine prn3lb

    This subroutine prints out information when either a built-in
      convergence test is satisfied or when an error message is
      generated.


                          *  *  *

    NEOS, November 1994. (Latest revision June 1996.)
    Optimization Technology Center.
    Argonne National Laboratory and Northwestern University.
    Written by
                       Ciyou Zhu
    in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal.


    ************
    */
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__;
    /* Parameter adjustments */
    --x;

    /* Function Body */
    /*     if (s_cmp(task, "ERROR", (ftnlen)5, (ftnlen)5) == 0) { */
    if ( IS_ERROR(*task) ) {
        goto L999;
    }
    if (*iprint >= 1) {


        printf("           * * * \n");
        printf("Tit   = total number of iterations\n");
        printf("Tnf   = total number of function evaluations\n");
        printf("Tnint = total number of segments explored during Cauchy searches\n");
        printf("Skip  = number of BFGS updates skipped\n");
        printf("Nact  = number of active bounds at final generalized Cauchy point\n");
        printf("Projg = norm of the final projected gradient\n");
        printf("F     = final function value\n");
        printf("           * * * \n");

        printf("   N    Tit   Tnf  Tnint  Skip  Nact      Projg        F\n");
        printf("%5ld %5ld %5ld %5ld %5ld %5ld\t%6.2e %9.5e\n",
               *n, *iter, *nfgv, *nintol, *nskip, *nact, *sbgnrm, *f );
        if (*iprint >= 100) {
            printf("X = ");
            i__1 = *n;
            for (i__ = 1; i__ <= i__1; ++i__) {
                printf(" %.2e", x[i__] );
            }
            printf("\n");
        }
        if (*iprint >= 1) {
            printf("F(x) = %.9e\n", *f );
        }
    }
L999:
    if (*iprint >= 1) {
        printf("%ld\n",*task);
        if (*info != 0) {
            if (*info == -1) {
                printf(" Matrix in 1st Cholesky factorization in formk is not Pos. Def.\n");
            }
            if (*info == -2) {
                printf(" Matrix in 2nd Cholesky factorization in formk is not Pos. Def.\n");
            }
            if (*info == -3) {
                printf(" Matrix in the Cholesky factorization in formt is not Pos. Def.\n");
            }
            if (*info == -4) {
                printf(" Derivative >= 0, backtracking line search impossible.\n");
                printf("  Previous x, f and g restored.\n");
                printf(" Possible causes: 1 error in function or gradient evaluation;\n");
                printf("                  2 rounding errors dominate computation.\n");
            }
            if (*info == -5) {
                printf(" Warning:  more than 10 function and gradient\n");
                printf("   evaluations in the last line search.  Termination\n");
                printf("   may possibly be caused by a bad search direction.\n");
            }
            if (*info == -6) {
                printf(" Input nbd(%ld) is invalid\n", *k );
            }
            if (*info == -7) {
                printf(" l(%ld) > u(%ld). No feasible solution.\n", *k, *k );
            }
            if (*info == -8) {
                printf(" The triangular system is singular.\n");
            }
            if (*info == -9) {
                printf(" Line search cannot locate an adequate point after 20 function\n");
                printf("  and gradient evaluations.  Previous x, f and g restored.\n");
                printf(" Possible causes: 1 error in function or gradient evaluation;\n");
                printf("                  2 rounding error dominate computation.\n");
            }
        }

        if (*iprint >= 1) {
            printf("Cauchy                time %.3e seconds.\n", *cachyt );
            printf("Subspace minimization time %.3e seconds.\n", *sbtime );
            printf("Line search           time %.3e seconds.\n", *lnscht );
        }
        printf(" Total User time %.3e seconds.\n", *time );
    }
    return 0;
} /* prn3lb */


int Lbfgs::errclb(integer *n, integer *m, double *factr,
                  double *l, double *u, integer *nbd, integer *task, integer *info,
                  integer *k, ftnlen task_len)
{
    /*
    ************

    Subroutine errclb

    This subroutine checks the validity of the input data.


                          *  *  *

    NEOS, November 1994. (Latest revision June 1996.)
    Optimization Technology Center.
    Argonne National Laboratory and Northwestern University.
    Written by
                       Ciyou Zhu
    in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal.


    ************
    */
    /*     Check the input arguments for errors. */
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__;

    /* Parameter adjustments */
    --nbd;
    --u;
    --l;

    /* Function Body */
    if (*n <= 0) *task = ERROR_N0;
    if (*m <= 0) *task = ERROR_M0;
    if (*factr < 0.)  *task = ERROR_FACTR;
    /*     Check the validity of the arrays nbd(i), u(i), and l(i). */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
        if (nbd[i__] < 0 || nbd[i__] > 3) {
            /*   return */
            *task = ERROR_NBD;
            *info = -6;
            *k = i__;
        }
        if (nbd[i__] == 2) {
            if (l[i__] > u[i__]) {
                /* return */
                *task = ERROR_FEAS;
                *info = -7;
                *k = i__;
            }
        }
        /* L10: */
    }
    return 0;
}


static integer c__11 = 11;

int Lbfgs::active(integer *n, double *l, double *u,
                  integer *nbd, double *x, integer *iwhere, integer *iprint,
                  logical *prjctd, logical *cnstnd, logical *boxed)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, nbdd;


    /*     ************ */

    /*     Subroutine active */

    /*     This subroutine initializes iwhere and projects the initial x to */
    /*       the feasible set if necessary. */

    /*     iwhere is an integer array of dimension n. */
    /*       On entry iwhere is unspecified. */
    /*       On exit iwhere(i)=-1  if x(i) has no bounds */
    /*                         3   if l(i)=u(i) */
    /*                         0   otherwise. */
    /*       In cauchy, iwhere is given finer gradations. */


    /*                           *  *  * */

    /*     NEOS, November 1994. (Latest revision June 1996.) */
    /*     Optimization Technology Center. */
    /*     Argonne National Laboratory and Northwestern University. */
    /*     Written by */
    /*                        Ciyou Zhu */
    /*     in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. */


    /*     ************ */
    /*     Initialize nbdd, prjctd, cnstnd and boxed. */
    /* Parameter adjustments */
    --iwhere;
    --x;
    --nbd;
    --u;
    --l;

    /* Function Body */
    nbdd = 0;
    *prjctd = FALSE_;
    *cnstnd = FALSE_;
    *boxed = TRUE_;
    /*     Project the initial x to the easible set if necessary. */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
        if (nbd[i__] > 0) {
            if (nbd[i__] <= 2 && x[i__] <= l[i__]) {
                if (x[i__] < l[i__]) {
                    *prjctd = TRUE_;
                    x[i__] = l[i__];
                }
                ++nbdd;
            } else if (nbd[i__] >= 2 && x[i__] >= u[i__]) {
                if (x[i__] > u[i__]) {
                    *prjctd = TRUE_;
                    x[i__] = u[i__];
                }
                ++nbdd;
            }
        }
        /* L10: */
    }
    /*     Initialize iwhere and assign values to cnstnd and boxed. */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
        if (nbd[i__] != 2) {
            *boxed = FALSE_;
        }
        if (nbd[i__] == 0) {
            /*                                this variable is always free */
            iwhere[i__] = -1;
            /*           otherwise set x(i)=mid(x(i), u(i), l(i)). */
        } else {
            *cnstnd = TRUE_;
            if (nbd[i__] == 2 && u[i__] - l[i__] <= 0.) {
                /*                   this variable is always fixed */
                iwhere[i__] = 3;
            } else {
                iwhere[i__] = 0;
            }
        }
        /* L20: */
    }
    if (*iprint >= 1) {
        if (*prjctd) {
            printf("The initial X is infeasible. Restart with its projection\n");
        }
        if (! (*cnstnd)) {
            printf("This problem is unconstrained\n");
        }
    }
    if (*iprint > 0) {
        printf("At X0, %ld variables are exactly at the bounds\n",nbdd);
    }
    return 0;
}

int Lbfgs::bmv(integer *m, double *sy, double *wt, integer
                                                       *col, double *v, double *p, integer *info)
{
    /* System generated locals */
    integer sy_dim1, sy_offset, wt_dim1, wt_offset, i__1, i__2;



    /* Local variables */
    integer i__, k, i2;
    double sum;

    /*     ************ */

    /*     Subroutine bmv */

    /*     This subroutine computes the product of the 2m x 2m middle matrix */
    /*       in the compact L-BFGS formula of B and a 2m vector v; */
    /*       it returns the product in p. */

    /*     m is an integer variable. */
    /*       On entry m is the maximum number of variable metric corrections */
    /*         used to define the limited memory matrix. */
    /*       On exit m is unchanged. */

    /*     sy is a double precision array of dimension m x m. */
    /*       On entry sy specifies the matrix S'Y. */
    /*       On exit sy is unchanged. */

    /*     wt is a double precision array of dimension m x m. */
    /*       On entry wt specifies the upper triangular matrix J' which is */
    /*         the Cholesky factor of (thetaS'S+LD^(-1)L'). */
    /*       On exit wt is unchanged. */

    /*     col is an integer variable. */
    /*       On entry col specifies the number of s-vectors (or y-vectors) */
    /*         stored in the compact L-BFGS formula. */
    /*       On exit col is unchanged. */

    /*     v is a double precision array of dimension 2col. */
    /*       On entry v specifies vector v. */
    /*       On exit v is unchanged. */

    /*     p is a double precision array of dimension 2col. */
    /*       On entry p is unspecified. */
    /*       On exit p is the product Mv. */

    /*     info is an integer variable. */
    /*       On entry info is unspecified. */
    /*       On exit info = 0       for normal return, */
    /*                    = nonzero for abnormal return when the system */
    /*                                to be solved by dtrsl is singular. */

    /*     Subprograms called: */

    /*       Linpack ... dtrsl. */


    /*                           *  *  * */

    /*     NEOS, November 1994. (Latest revision June 1996.) */
    /*     Optimization Technology Center. */
    /*     Argonne National Laboratory and Northwestern University. */
    /*     Written by */
    /*                        Ciyou Zhu */
    /*     in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. */


    /*     ************ */
    /* Parameter adjustments */
    wt_dim1 = *m;
    wt_offset = 1 + wt_dim1;
    wt -= wt_offset;
    sy_dim1 = *m;
    sy_offset = 1 + sy_dim1;
    sy -= sy_offset;
    --p;
    --v;

    /* Function Body */
    if (*col == 0) {
        return 0;
    }
    /*     PART I: solve [  D^(1/2)      O ] [ p1 ] = [ v1 ] */
    /*                   [ -L*D^(-1/2)   J ] [ p2 ]   [ v2 ]. */
    /*       solve Jp2=v2+LD^(-1)v1. */
    p[*col + 1] = v[*col + 1];
    i__1 = *col;
    for (i__ = 2; i__ <= i__1; ++i__) {
        i2 = *col + i__;
        sum = 0.;
        i__2 = i__ - 1;
        for (k = 1; k <= i__2; ++k) {
            sum += sy[i__ + k * sy_dim1] * v[k] / sy[k + k * sy_dim1];
            /* L10: */
        }
        p[i2] = v[i2] + sum;
        /* L20: */
    }
    /*     Solve the triangular system */
    dtrsl(&wt[wt_offset], m, col, &p[*col + 1], &c__11, info);
    if (*info != 0) {
        return 0;
    }
    /*       solve D^(1/2)p1=v1. */
    i__1 = *col;
    for (i__ = 1; i__ <= i__1; ++i__) {
        p[i__] = v[i__] / sqrt(sy[i__ + i__ * sy_dim1]);
        /* L30: */
    }
    /*     PART II: solve [ -D^(1/2)   D^(-1/2)*L'  ] [ p1 ] = [ p1 ] */
    /*                    [  0         J'           ] [ p2 ]   [ p2 ]. */
    /*       solve J^Tp2=p2. */
    dtrsl(&wt[wt_offset], m, col, &p[*col + 1], &c__1, info);
    if (*info != 0) {
        return 0;
    }
    /*       compute p1=-D^(-1/2)(p1-D^(-1/2)L'p2) */
    /*                 =-D^(-1/2)p1+D^(-1)L'p2. */
    i__1 = *col;
    for (i__ = 1; i__ <= i__1; ++i__) {
        p[i__] = -p[i__] / sqrt(sy[i__ + i__ * sy_dim1]);
        /* L40: */
    }
    i__1 = *col;
    for (i__ = 1; i__ <= i__1; ++i__) {
        sum = 0.;
        i__2 = *col;
        for (k = i__ + 1; k <= i__2; ++k) {
            sum += sy[k + i__ * sy_dim1] * p[*col + k] / sy[i__ + i__ *
                                                                      sy_dim1];
            /* L50: */
        }
        p[i__] += sum;
        /* L60: */
    }
    return 0;
} /* bmv */


int Lbfgs::cauchy(integer *n, double *x, double *l,
                  double *u, integer *nbd, double *g, integer *iorder, integer *
                      iwhere, double *t, double *d__, double *xcp, integer *m,
                  double *wy, double *ws, double *sy, double *wt,
                  double *theta, integer *col, integer *head, double *p,
                  double *c__, double *wbp, double *v, integer *nseg,
                  integer *iprint, double *sbgnrm, integer *info, double *
                      epsmch)
{

    /* System generated locals */
    integer wy_dim1, wy_offset, ws_dim1, ws_offset, sy_dim1, sy_offset,
        wt_dim1, wt_offset, i__1, i__2;
    double d__1;


    /* Local variables */
    integer i__, j;
    double f1, f2, dt, tj, tl, tu, tj0;
    integer ibp;
    double dtm;
    double wmc, wmp, wmw;
    integer col2;
    double dibp;
    integer iter;
    double zibp, tsum, dibp2;
    logical bnded;
    double neggi;
    integer nfree;
    double bkmin;
    integer nleft;
    double f2_org__;
    integer nbreak, ibkmin;
    integer pointr;
    logical xlower, xupper;

    /*     ************ */

    /*     Subroutine cauchy */

    /*     For given x, l, u, g (with sbgnrm > 0), and a limited memory */
    /*       BFGS matrix B defined in terms of matrices WY, WS, WT, and */
    /*       scalars head, col, and theta, this subroutine computes the */
    /*       generalized Cauchy point (GCP), defined as the first local */
    /*       minimizer of the quadratic */

    /*                  Q(x + s) = g's + 1/2 s'Bs */

    /*       along the projected gradient direction P(x-tg,l,u). */
    /*       The routine returns the GCP in xcp. */

    /*     n is an integer variable. */
    /*       On entry n is the dimension of the problem. */
    /*       On exit n is unchanged. */

    /*     x is a double precision array of dimension n. */
    /*       On entry x is the starting point for the GCP computation. */
    /*       On exit x is unchanged. */

    /*     l is a double precision array of dimension n. */
    /*       On entry l is the lower bound of x. */
    /*       On exit l is unchanged. */

    /*     u is a double precision array of dimension n. */
    /*       On entry u is the upper bound of x. */
    /*       On exit u is unchanged. */

    /*     nbd is an integer array of dimension n. */
    /*       On entry nbd represents the type of bounds imposed on the */
    /*         variables, and must be specified as follows: */
    /*         nbd(i)=0 if x(i) is unbounded, */
    /*                1 if x(i) has only a lower bound, */
    /*                2 if x(i) has both lower and upper bounds, and */
    /*                3 if x(i) has only an upper bound. */
    /*       On exit nbd is unchanged. */

    /*     g is a double precision array of dimension n. */
    /*       On entry g is the gradient of f(x).  g must be a nonzero vector. */
    /*       On exit g is unchanged. */

    /*     iorder is an integer working array of dimension n. */
    /*       iorder will be used to store the breakpoints in the piecewise */
    /*       linear path and free variables encountered. On exit, */
    /*         iorder(1),...,iorder(nleft) are indices of breakpoints */
    /*                                which have not been encountered; */
    /*         iorder(nleft+1),...,iorder(nbreak) are indices of */
    /*                                     encountered breakpoints; and */
    /*         iorder(nfree),...,iorder(n) are indices of variables which */
    /*                 have no bound constraits along the search direction. */

    /*     iwhere is an integer array of dimension n. */
    /*       On entry iwhere indicates only the permanently fixed (iwhere=3) */
    /*       or free (iwhere= -1) components of x. */
    /*       On exit iwhere records the status of the current x variables. */
    /*       iwhere(i)=-3  if x(i) is free and has bounds, but is not moved */
    /*                 0   if x(i) is free and has bounds, and is moved */
    /*                 1   if x(i) is fixed at l(i), and l(i) .ne. u(i) */
    /*                 2   if x(i) is fixed at u(i), and u(i) .ne. l(i) */
    /*                 3   if x(i) is always fixed, i.e.,  u(i)=x(i)=l(i) */
    /*                 -1  if x(i) is always free, i.e., it has no bounds. */

    /*     t is a double precision working array of dimension n. */
    /*       t will be used to store the break points. */

    /*     d is a double precision array of dimension n used to store */
    /*       the Cauchy direction P(x-tg)-x. */

    /*     xcp is a double precision array of dimension n used to return the */
    /*       GCP on exit. */

    /*     m is an integer variable. */
    /*       On entry m is the maximum number of variable metric corrections */
    /*         used to define the limited memory matrix. */
    /*       On exit m is unchanged. */

    /*     ws, wy, sy, and wt are double precision arrays. */
    /*       On entry they store information that defines the */
    /*                             limited memory BFGS matrix: */
    /*         ws(n,m) stores S, a set of s-vectors; */
    /*         wy(n,m) stores Y, a set of y-vectors; */
    /*         sy(m,m) stores S'Y; */
    /*         wt(m,m) stores the */
    /*                 Cholesky factorization of (theta*S'S+LD^(-1)L'). */
    /*       On exit these arrays are unchanged. */

    /*     theta is a double precision variable. */
    /*       On entry theta is the scaling factor specifying B_0 = theta I. */
    /*       On exit theta is unchanged. */

    /*     col is an integer variable. */
    /*       On entry col is the actual number of variable metric */
    /*         corrections stored so far. */
    /*       On exit col is unchanged. */

    /*     head is an integer variable. */
    /*       On entry head is the location of the first s-vector (or y-vector) */
    /*         in S (or Y). */
    /*       On exit col is unchanged. */

    /*     p is a double precision working array of dimension 2m. */
    /*       p will be used to store the vector p = W^(T)d. */

    /*     c is a double precision working array of dimension 2m. */
    /*       c will be used to store the vector c = W^(T)(xcp-x). */

    /*     wbp is a double precision working array of dimension 2m. */
    /*       wbp will be used to store the row of W corresponding */
    /*         to a breakpoint. */

    /*     v is a double precision working array of dimension 2m. */

    /*     nseg is an integer variable. */
    /*       On exit nseg records the number of quadratic segments explored */
    /*         in searching for the GCP. */

    /*     sg and yg are double precision arrays of dimension m. */
    /*       On entry sg  and yg store S'g and Y'g correspondingly. */
    /*       On exit they are unchanged. */

    /*     iprint is an INTEGER variable that must be set by the user. */
    /*       It controls the frequency and type of output generated: */
    /*        iprint<0    no output is generated; */
    /*        iprint=0    print only one line at the last iteration; */
    /*        0<iprint<99 print also f and |proj g| every iprint iterations; */
    /*        iprint=99   print details of every iteration except n-vectors; */
    /*        iprint=100  print also the changes of active set and final x; */
    /*        iprint>100  print details of every iteration including x and g; */
    /*       When iprint > 0, the file iterate.dat will be created to */
    /*                        summarize the iteration. */

    /*     sbgnrm is a double precision variable. */
    /*       On entry sbgnrm is the norm of the projected gradient at x. */
    /*       On exit sbgnrm is unchanged. */

    /*     info is an integer variable. */
    /*       On entry info is 0. */
    /*       On exit info = 0       for normal return, */
    /*                    = nonzero for abnormal return when the the system */
    /*                              used in routine bmv is singular. */

    /*     Subprograms called: */

    /*       L-BFGS-B Library ... hpsolb, bmv. */

    /*       Linpack ... dscal dcopy, daxpy. */


    /*     References: */

    /*       [1] R. H. Byrd, P. Lu, J. Nocedal and C. Zhu, ``A limited */
    /*       memory algorithm for bound constrained optimization'', */
    /*       SIAM J. Scientific Computing 16 (1995), no. 5, pp. 1190--1208. */

    /*       [2] C. Zhu, R.H. Byrd, P. Lu, J. Nocedal, ``L-BFGS-B: FORTRAN */
    /*       Subroutines for Large Scale Bound Constrained Optimization'' */
    /*       Tech. Report, NAM-11, EECS Department, Northwestern University, */
    /*       1994. */

    /*       (Postscript files of these papers are available via anonymous */
    /*        ftp to eecs.nwu.edu in the directory pub/lbfgs/lbfgs_bcm.) */

    /*                           *  *  * */

    /*     NEOS, November 1994. (Latest revision June 1996.) */
    /*     Optimization Technology Center. */
    /*     Argonne National Laboratory and Northwestern University. */
    /*     Written by */
    /*                        Ciyou Zhu */
    /*     in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. */


    /*     ************ */
    /*     Check the status of the variables, reset iwhere(i) if necessary; */
    /*       compute the Cauchy direction d and the breakpoints t; initialize */
    /*       the derivative f1 and the vector p = W'd (for theta = 1). */
    /* Parameter adjustments */
    --xcp;
    --d__;
    --t;
    --iwhere;
    --iorder;
    --g;
    --nbd;
    --u;
    --l;
    --x;
    --v;
    --wbp;
    --c__;
    --p;
    wt_dim1 = *m;
    wt_offset = 1 + wt_dim1;
    wt -= wt_offset;
    sy_dim1 = *m;
    sy_offset = 1 + sy_dim1;
    sy -= sy_offset;
    ws_dim1 = *n;
    ws_offset = 1 + ws_dim1;
    ws -= ws_offset;
    wy_dim1 = *n;
    wy_offset = 1 + wy_dim1;
    wy -= wy_offset;

    /* Function Body */
    if (*sbgnrm <= 0.) {
        if (*iprint >= 1) {
            printf("Subnorm = 0. GCP = X.\n");
        }
        dcopy(n, &x[1], &c__1, &xcp[1], &c__1);
        return 0;
    }
    bnded = TRUE_;
    nfree = *n + 1;
    nbreak = 0;
    ibkmin = 0;
    bkmin = 0.;
    col2 = *col << 1;
    f1 = 0.;
    if (*iprint >= 99) {
        printf("CAUCHY entered\n");
    }

    /*     We set p to zero and build it up as we determine d. */
    i__1 = col2;
    for (i__ = 1; i__ <= i__1; ++i__) {
        p[i__] = 0.;
        /* L20: */
    }
    /*     In the following loop we determine for each variable its bound */
    /*        status and its breakpoint, and update p accordingly. */
    /*        Smallest breakpoint is identified. */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
        neggi = -g[i__];
        if (iwhere[i__] != 3 && iwhere[i__] != -1) {
            /*             if x(i) is not a constant and has bounds, */
            /*             compute the difference between x(i) and its bounds. */
            if (nbd[i__] <= 2) {
                tl = x[i__] - l[i__];
            }
            if (nbd[i__] >= 2) {
                tu = u[i__] - x[i__];
            }
            /*           If a variable is close enough to a bound */
            /*             we treat it as at bound. */
            xlower = nbd[i__] <= 2 && tl <= 0.;
            xupper = nbd[i__] >= 2 && tu <= 0.;
            /*              reset iwhere(i). */
            iwhere[i__] = 0;
            if (xlower) {
                if (neggi <= 0.) {
                    iwhere[i__] = 1;
                }
            } else if (xupper) {
                if (neggi >= 0.) {
                    iwhere[i__] = 2;
                }
            } else {
                if (fabs(neggi) <= 0.) {
                    iwhere[i__] = -3;
                }
            }
        }
        pointr = *head;
        if (iwhere[i__] != 0 && iwhere[i__] != -1) {
            d__[i__] = 0.;
        } else {
            d__[i__] = neggi;
            f1 -= neggi * neggi;
            /*             calculate p := p - W'e_i* (g_i). */
            i__2 = *col;
            for (j = 1; j <= i__2; ++j) {
                p[j] += wy[i__ + pointr * wy_dim1] * neggi;
                p[*col + j] += ws[i__ + pointr * ws_dim1] * neggi;
                pointr = pointr % *m + 1;
                /* L40: */
            }
            if (nbd[i__] <= 2 && nbd[i__] != 0 && neggi < 0.) {
                /*                                 x(i) + d(i) is bounded; compute t(i). */
                ++nbreak;
                iorder[nbreak] = i__;
                t[nbreak] = tl / (-neggi);
                if (nbreak == 1 || t[nbreak] < bkmin) {
                    bkmin = t[nbreak];
                    ibkmin = nbreak;
                }
            } else if (nbd[i__] >= 2 && neggi > 0.) {
                /*                                 x(i) + d(i) is bounded; compute t(i). */
                ++nbreak;
                iorder[nbreak] = i__;
                t[nbreak] = tu / neggi;
                if (nbreak == 1 || t[nbreak] < bkmin) {
                    bkmin = t[nbreak];
                    ibkmin = nbreak;
                }
            } else {
                /*                x(i) + d(i) is not bounded. */
                --nfree;
                iorder[nfree] = i__;
                if (fabs(neggi) > 0.) {
                    bnded = FALSE_;
                }
            }
        }
        /* L50: */
    }
    /*     The indices of the nonzero components of d are now stored */
    /*       in iorder(1),...,iorder(nbreak) and iorder(nfree),...,iorder(n). */
    /*       The smallest of the nbreak breakpoints is in t(ibkmin)=bkmin. */
    if (*theta != 1.) {
        /*                   complete the initialization of p for theta not= one. */
        dscal(col, theta, &p[*col + 1], &c__1);
    }
    /*     Initialize GCP xcp = x. */
    dcopy(n, &x[1], &c__1, &xcp[1], &c__1);
    if (nbreak == 0 && nfree == *n + 1) {
        /*                  is a zero vector, return with the initial xcp as GCP. */
        if (*iprint > 100) {
            printf("Cauchy X = ");
            i__1 = *n;
            for (i__ = 1; i__ <= i__1; ++i__) {
                printf("%5.2e ", xcp[i__] );
            }
            printf("\n");
        }
        return 0;
    }
    /*     Initialize c = W'(xcp - x) = 0. */
    i__1 = col2;
    for (j = 1; j <= i__1; ++j) {
        c__[j] = 0.;
        /* L60: */
    }
    /*     Initialize derivative f2. */
    f2 = -(*theta) * f1;
    f2_org__ = f2;
    if (*col > 0) {
        bmv(m, &sy[sy_offset], &wt[wt_offset], col, &p[1], &v[1], info);
        if (*info != 0) {
            return 0;
        }
        f2 -= ddot(&col2, &v[1], &c__1, &p[1], &c__1);
    }
    dtm = -f1 / f2;
    tsum = 0.;
    *nseg = 1;
    if (*iprint >= 99) {
        printf("There are %ld breakpoints\n", nbreak );
    }
    /*     If there are no breakpoints, locate the GCP and return. */
    if (nbreak == 0) {
        goto L888;
    }
    nleft = nbreak;
    iter = 1;
    tj = 0.;
    /* ------------------- the beginning of the loop ------------------------- */
L777:
    /*     Find the next smallest breakpoint; */
    /*       compute dt = t(nleft) - t(nleft + 1). */
    tj0 = tj;
    if (iter == 1) {
        /*         Since we already have the smallest breakpoint we need not do */
        /*         heapsort yet. Often only one breakpoint is used and the */
        /*         cost of heapsort is avoided. */
        tj = bkmin;
        ibp = iorder[ibkmin];
    } else {
        if (iter == 2) {
            /*             Replace the already used smallest breakpoint with the */
            /*             breakpoint numbered nbreak > nlast, before heapsort call. */
            if (ibkmin != nbreak) {
                t[ibkmin] = t[nbreak];
                iorder[ibkmin] = iorder[nbreak];
            }
            /*        Update heap structure of breakpoints */
            /*           (if iter=2, initialize heap). */
        }
        i__1 = iter - 2;
        hpsolb(&nleft, &t[1], &iorder[1], &i__1);
        tj = t[nleft];
        ibp = iorder[nleft];
    }
    dt = tj - tj0;
    if (dt != 0. && *iprint >= 100) {
        printf("Piece %ld --f1, f2 at start point %.2e %.2e\n", *nseg, f1, f2 );
        printf("Distance to the next break point = %.2e\n", dt );
        printf("Distance to the stationary point = %.2e\n", dtm );
    }
    /*     If a minimizer is within this interval, locate the GCP and return. */
    if (dtm < dt) {
        goto L888;
    }
    /*     Otherwise fix one variable and */
    /*       reset the corresponding component of d to zero. */
    tsum += dt;
    --nleft;
    ++iter;
    dibp = d__[ibp];
    d__[ibp] = 0.;
    if (dibp > 0.) {
        zibp = u[ibp] - x[ibp];
        xcp[ibp] = u[ibp];
        iwhere[ibp] = 2;
    } else {
        zibp = l[ibp] - x[ibp];
        xcp[ibp] = l[ibp];
        iwhere[ibp] = 1;
    }
    if (*iprint >= 100) {
        printf("Variable %ld is fixed\n", ibp );
    }
    if (nleft == 0 && nbreak == *n) {
        /*                                             all n variables are fixed, */
        /*                                                return with xcp as GCP. */
        dtm = dt;
        goto L999;
    }
    /*     Update the derivative information. */
    ++(*nseg);
    /* Computing 2nd power */
    d__1 = dibp;
    dibp2 = d__1 * d__1;
    /*     Update f1 and f2. */
    /*        temporarily set f1 and f2 for col=0. */
    f1 = f1 + dt * f2 + dibp2 - *theta * dibp * zibp;
    f2 -= *theta * dibp2;
    if (*col > 0) {
        /*                          update c = c + dt*p. */
        daxpy(&col2, &dt, &p[1], &c__1, &c__[1], &c__1);
        /*           choose wbp, */
        /*           the row of W corresponding to the breakpoint encountered. */
        pointr = *head;
        i__1 = *col;
        for (j = 1; j <= i__1; ++j) {
            wbp[j] = wy[ibp + pointr * wy_dim1];
            wbp[*col + j] = *theta * ws[ibp + pointr * ws_dim1];
            pointr = pointr % *m + 1;
            /* L70: */
        }
        /*           compute (wbp)Mc, (wbp)Mp, and (wbp)M(wbp)'. */
        bmv(m, &sy[sy_offset], &wt[wt_offset], col, &wbp[1], &v[1], info);
        if (*info != 0) {
            return 0;
        }
        wmc = ddot(&col2, &c__[1], &c__1, &v[1], &c__1);
        wmp = ddot(&col2, &p[1], &c__1, &v[1], &c__1);
        wmw = ddot(&col2, &wbp[1], &c__1, &v[1], &c__1);
        /*           update p = p - dibp*wbp. */
        d__1 = -dibp;
        daxpy(&col2, &d__1, &wbp[1], &c__1, &p[1], &c__1);
        /*           complete updating f1 and f2 while col > 0. */
        f1 += dibp * wmc;
        f2 = f2 + dibp * 2. * wmp - dibp2 * wmw;
    }
    /* Computing MAX */
    d__1 = *epsmch * f2_org__;
    f2 = fmax(d__1,f2);
    if (nleft > 0) {
        dtm = -f1 / f2;
        goto L777;
        /*                 to repeat the loop for unsearched intervals. */
    } else if (bnded) {
        f1 = 0.;
        f2 = 0.;
        dtm = 0.;
    } else {
        dtm = -f1 / f2;
    }
    /* ------------------- the end of the loop ------------------------------- */
L888:
    if (*iprint >= 99) {
        printf("\nGCP found in this segment. Piece %ld --f1, f2 at start point %.2e %.2e\n", *nseg, f1, f2 );
        printf("Distance to the stationary point = %.2e\n", dtm );
    }
    if (dtm <= 0.) {
        dtm = 0.;
    }
    tsum += dtm;
    /*     Move free variables (i.e., the ones w/o breakpoints) and */
    /*       the variables whose breakpoints haven't been reached. */
    daxpy(n, &tsum, &d__[1], &c__1, &xcp[1], &c__1);
L999:
    /*     Update c = c + dtm*p = W'(x^c - x) */
    /*       which will be used in computing r = Z'(B(x^c - x) + g). */
    if (*col > 0) {
        daxpy(&col2, &dtm, &p[1], &c__1, &c__[1], &c__1);
    }
    if (*iprint > 100) {

        printf("Cauchy X = ");
        i__1 = *n;
        for (i__ = 1; i__ <= i__1; ++i__) {
            printf("%5.2e ", xcp[i__] );
        }
    }
    if (*iprint >= 99) {
        printf("-------------- exit CAUCHY -----------\n");
    }
    return 0;
}

int Lbfgs::cmprlb(integer *n, integer *m, double *x,
                  double *g, double *ws, double *wy, double *sy,
                  double *wt, double *z__, double *r__, double *wa,
                  integer *index, double *theta, integer *col, integer *head,
                  integer *nfree, logical *cnstnd, integer *info)
{
    /* System generated locals */
    integer ws_dim1, ws_offset, wy_dim1, wy_offset, sy_dim1, sy_offset,
        wt_dim1, wt_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, k;
    double a1, a2;
    integer pointr;

    /*     ************ */

    /*     Subroutine cmprlb */

    /*       This subroutine computes r=-Z'B(xcp-xk)-Z'g by using */
    /*         wa(2m+1)=W'(xcp-x) from subroutine cauchy. */

    /*     Subprograms called: */

    /*       L-BFGS-B Library ... bmv. */


    /*                           *  *  * */

    /*     NEOS, November 1994. (Latest revision June 1996.) */
    /*     Optimization Technology Center. */
    /*     Argonne National Laboratory and Northwestern University. */
    /*     Written by */
    /*                        Ciyou Zhu */
    /*     in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. */


    /*     ************ */
    /* Parameter adjustments */
    --index;
    --r__;
    --z__;
    --g;
    --x;
    --wa;
    wt_dim1 = *m;
    wt_offset = 1 + wt_dim1;
    wt -= wt_offset;
    sy_dim1 = *m;
    sy_offset = 1 + sy_dim1;
    sy -= sy_offset;
    wy_dim1 = *n;
    wy_offset = 1 + wy_dim1;
    wy -= wy_offset;
    ws_dim1 = *n;
    ws_offset = 1 + ws_dim1;
    ws -= ws_offset;

    /* Function Body */
    if (! (*cnstnd) && *col > 0) {
        i__1 = *n;
        for (i__ = 1; i__ <= i__1; ++i__) {
            r__[i__] = -g[i__];
            /* L26: */
        }
    } else {
        i__1 = *nfree;
        for (i__ = 1; i__ <= i__1; ++i__) {
            k = index[i__];
            r__[i__] = -(*theta) * (z__[k] - x[k]) - g[k];
            /* L30: */
        }
        bmv(m, &sy[sy_offset], &wt[wt_offset], col, &wa[(*m << 1) + 1], &wa[
                                                                            1], info);
        if (*info != 0) {
            *info = -8;
            return 0;
        }
        pointr = *head;
        i__1 = *col;
        for (j = 1; j <= i__1; ++j) {
            a1 = wa[j];
            a2 = *theta * wa[*col + j];
            i__2 = *nfree;
            for (i__ = 1; i__ <= i__2; ++i__) {
                k = index[i__];
                r__[i__] = r__[i__] + wy[k + pointr * wy_dim1] * a1 + ws[k +
                                                                         pointr * ws_dim1] * a2;
                /* L32: */
            }
            pointr = pointr % *m + 1;
            /* L34: */
        }
    }
    return 0;
}


int Lbfgs::formk(integer *n, integer *nsub, integer *ind, integer *
                                                              nenter, integer *ileave, integer *indx2, integer *iupdat, logical *
                     updatd, double *wn, double *wn1, integer *m, double *ws,
                 double *wy, double *sy, double *theta, integer *col,
                 integer *head, integer *info)
{
    /* System generated locals */
    integer wn_dim1, wn_offset, wn1_dim1, wn1_offset, ws_dim1, ws_offset,
        wy_dim1, wy_offset, sy_dim1, sy_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__, k, k1, m2, is, js, iy, jy, is1, js1, col2, dend, pend;
    integer upcl;
    double temp1, temp2, temp3, temp4;
    integer ipntr, jpntr, dbegin, pbegin;

    /*     ************ */

    /*     Subroutine formk */

    /*     This subroutine forms  the LEL^T factorization of the indefinite */

    /*       matrix    K = [-D -Y'ZZ'Y/theta     L_a'-R_z'  ] */
    /*                     [L_a -R_z           theta*S'AA'S ] */
    /*                                                    where E = [-I  0] */
    /*                                                              [ 0  I] */
    /*     The matrix K can be shown to be equal to the matrix M^[-1]N */
    /*       occurring in section 5.1 of [1], as well as to the matrix */
    /*       Mbar^[-1] Nbar in section 5.3. */

    /*     n is an integer variable. */
    /*       On entry n is the dimension of the problem. */
    /*       On exit n is unchanged. */

    /*     nsub is an integer variable */
    /*       On entry nsub is the number of subspace variables in free set. */
    /*       On exit nsub is not changed. */

    /*     ind is an integer array of dimension nsub. */
    /*       On entry ind specifies the indices of subspace variables. */
    /*       On exit ind is unchanged. */

    /*     nenter is an integer variable. */
    /*       On entry nenter is the number of variables entering the */
    /*         free set. */
    /*       On exit nenter is unchanged. */

    /*     ileave is an integer variable. */
    /*       On entry indx2(ileave),...,indx2(n) are the variables leaving */
    /*         the free set. */
    /*       On exit ileave is unchanged. */

    /*     indx2 is an integer array of dimension n. */
    /*       On entry indx2(1),...,indx2(nenter) are the variables entering */
    /*         the free set, while indx2(ileave),...,indx2(n) are the */
    /*         variables leaving the free set. */
    /*       On exit indx2 is unchanged. */

    /*     iupdat is an integer variable. */
    /*       On entry iupdat is the total number of BFGS updates made so far. */
    /*       On exit iupdat is unchanged. */

    /*     updatd is a logical variable. */
    /*       On entry 'updatd' is true if the L-BFGS matrix is updatd. */
    /*       On exit 'updatd' is unchanged. */

    /*     wn is a double precision array of dimension 2m x 2m. */
    /*       On entry wn is unspecified. */
    /*       On exit the upper triangle of wn stores the LEL^T factorization */
    /*         of the 2*col x 2*col indefinite matrix */
    /*                     [-D -Y'ZZ'Y/theta     L_a'-R_z'  ] */
    /*                     [L_a -R_z           theta*S'AA'S ] */

    /*     wn1 is a double precision array of dimension 2m x 2m. */
    /*       On entry wn1 stores the lower triangular part of */
    /*                     [Y' ZZ'Y   L_a'+R_z'] */
    /*                     [L_a+R_z   S'AA'S   ] */
    /*         in the previous iteration. */
    /*       On exit wn1 stores the corresponding updated matrices. */
    /*       The purpose of wn1 is just to store these inner products */
    /*       so they can be easily updated and inserted into wn. */

    /*     m is an integer variable. */
    /*       On entry m is the maximum number of variable metric corrections */
    /*         used to define the limited memory matrix. */
    /*       On exit m is unchanged. */

    /*     ws, wy, sy, and wtyy are double precision arrays; */
    /*     theta is a double precision variable; */
    /*     col is an integer variable; */
    /*     head is an integer variable. */
    /*       On entry they store the information defining the */
    /*                                          limited memory BFGS matrix: */
    /*         ws(n,m) stores S, a set of s-vectors; */
    /*         wy(n,m) stores Y, a set of y-vectors; */
    /*         sy(m,m) stores S'Y; */
    /*         wtyy(m,m) stores the Cholesky factorization */
    /*                                   of (theta*S'S+LD^(-1)L') */
    /*         theta is the scaling factor specifying B_0 = theta I; */
    /*         col is the number of variable metric corrections stored; */
    /*         head is the location of the 1st s- (or y-) vector in S (or Y). */
    /*       On exit they are unchanged. */

    /*     info is an integer variable. */
    /*       On entry info is unspecified. */
    /*       On exit info =  0 for normal return; */
    /*                    = -1 when the 1st Cholesky factorization failed; */
    /*                    = -2 when the 2st Cholesky factorization failed. */

    /*     Subprograms called: */

    /*       Linpack ... dcopy, dpofa, dtrsl. */


    /*     References: */
    /*       [1] R. H. Byrd, P. Lu, J. Nocedal and C. Zhu, ``A limited */
    /*       memory algorithm for bound constrained optimization'', */
    /*       SIAM J. Scientific Computing 16 (1995), no. 5, pp. 1190--1208. */

    /*       [2] C. Zhu, R.H. Byrd, P. Lu, J. Nocedal, ``L-BFGS-B: a */
    /*       limited memory FORTRAN code for solving bound constrained */
    /*       optimization problems'', Tech. Report, NAM-11, EECS Department, */
    /*       Northwestern University, 1994. */

    /*       (Postscript files of these papers are available via anonymous */
    /*        ftp to eecs.nwu.edu in the directory pub/lbfgs/lbfgs_bcm.) */

    /*                           *  *  * */

    /*     NEOS, November 1994. (Latest revision June 1996.) */
    /*     Optimization Technology Center. */
    /*     Argonne National Laboratory and Northwestern University. */
    /*     Written by */
    /*                        Ciyou Zhu */
    /*     in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. */


    /*     ************ */
    /*     Form the lower triangular part of */
    /*               WN1 = [Y' ZZ'Y   L_a'+R_z'] */
    /*                     [L_a+R_z   S'AA'S   ] */
    /*        where L_a is the strictly lower triangular part of S'AA'Y */
    /*              R_z is the upper triangular part of S'ZZ'Y. */
    /* Parameter adjustments */
    --indx2;
    --ind;
    sy_dim1 = *m;
    sy_offset = 1 + sy_dim1;
    sy -= sy_offset;
    wy_dim1 = *n;
    wy_offset = 1 + wy_dim1;
    wy -= wy_offset;
    ws_dim1 = *n;
    ws_offset = 1 + ws_dim1;
    ws -= ws_offset;
    wn1_dim1 = 2 * *m;
    wn1_offset = 1 + wn1_dim1;
    wn1 -= wn1_offset;
    wn_dim1 = 2 * *m;
    wn_offset = 1 + wn_dim1;
    wn -= wn_offset;

    /* Function Body */
    if (*updatd) {
        if (*iupdat > *m) {
            /*                                 shift old part of WN1. */
            i__1 = *m - 1;
            for (jy = 1; jy <= i__1; ++jy) {
                js = *m + jy;
                i__2 = *m - jy;
                dcopy(&i__2, &wn1[jy + 1 + (jy + 1) * wn1_dim1], &c__1, &wn1[
                                                                            jy + jy * wn1_dim1], &c__1);
                i__2 = *m - jy;
                dcopy(&i__2, &wn1[js + 1 + (js + 1) * wn1_dim1], &c__1, &wn1[
                                                                            js + js * wn1_dim1], &c__1);
                i__2 = *m - 1;
                dcopy(&i__2, &wn1[*m + 2 + (jy + 1) * wn1_dim1], &c__1, &wn1[
                                                                            *m + 1 + jy * wn1_dim1], &c__1);
                /* L10: */
            }
        }
        /*          put new rows in blocks (1,1), (2,1) and (2,2). */
        pbegin = 1;
        pend = *nsub;
        dbegin = *nsub + 1;
        dend = *n;
        iy = *col;
        is = *m + *col;
        ipntr = *head + *col - 1;
        if (ipntr > *m) {
            ipntr -= *m;
        }
        jpntr = *head;
        i__1 = *col;
        for (jy = 1; jy <= i__1; ++jy) {
            js = *m + jy;
            temp1 = 0.;
            temp2 = 0.;
            temp3 = 0.;
            /*             compute element jy of row 'col' of Y'ZZ'Y */
            i__2 = pend;
            for (k = pbegin; k <= i__2; ++k) {
                k1 = ind[k];
                temp1 += wy[k1 + ipntr * wy_dim1] * wy[k1 + jpntr * wy_dim1];
                /* L15: */
            }
            /*             compute elements jy of row 'col' of L_a and S'AA'S */
            i__2 = dend;
            for (k = dbegin; k <= i__2; ++k) {
                k1 = ind[k];
                temp2 += ws[k1 + ipntr * ws_dim1] * ws[k1 + jpntr * ws_dim1];
                temp3 += ws[k1 + ipntr * ws_dim1] * wy[k1 + jpntr * wy_dim1];
                /* L16: */
            }
            wn1[iy + jy * wn1_dim1] = temp1;
            wn1[is + js * wn1_dim1] = temp2;
            wn1[is + jy * wn1_dim1] = temp3;
            jpntr = jpntr % *m + 1;
            /* L20: */
        }
        /*          put new column in block (2,1). */
        jy = *col;
        jpntr = *head + *col - 1;
        if (jpntr > *m) {
            jpntr -= *m;
        }
        ipntr = *head;
        i__1 = *col;
        for (i__ = 1; i__ <= i__1; ++i__) {
            is = *m + i__;
            temp3 = 0.;
            /*             compute element i of column 'col' of R_z */
            i__2 = pend;
            for (k = pbegin; k <= i__2; ++k) {
                k1 = ind[k];
                temp3 += ws[k1 + ipntr * ws_dim1] * wy[k1 + jpntr * wy_dim1];
                /* L25: */
            }
            ipntr = ipntr % *m + 1;
            wn1[is + jy * wn1_dim1] = temp3;
            /* L30: */
        }
        upcl = *col - 1;
    } else {
        upcl = *col;
    }
    /*       modify the old parts in blocks (1,1) and (2,2) due to changes */
    /*       in the set of free variables. */
    ipntr = *head;
    i__1 = upcl;
    for (iy = 1; iy <= i__1; ++iy) {
        is = *m + iy;
        jpntr = *head;
        i__2 = iy;
        for (jy = 1; jy <= i__2; ++jy) {
            js = *m + jy;
            temp1 = 0.;
            temp2 = 0.;
            temp3 = 0.;
            temp4 = 0.;
            i__3 = *nenter;
            for (k = 1; k <= i__3; ++k) {
                k1 = indx2[k];
                temp1 += wy[k1 + ipntr * wy_dim1] * wy[k1 + jpntr * wy_dim1];
                temp2 += ws[k1 + ipntr * ws_dim1] * ws[k1 + jpntr * ws_dim1];
                /* L35: */
            }
            i__3 = *n;
            for (k = *ileave; k <= i__3; ++k) {
                k1 = indx2[k];
                temp3 += wy[k1 + ipntr * wy_dim1] * wy[k1 + jpntr * wy_dim1];
                temp4 += ws[k1 + ipntr * ws_dim1] * ws[k1 + jpntr * ws_dim1];
                /* L36: */
            }
            wn1[iy + jy * wn1_dim1] = wn1[iy + jy * wn1_dim1] + temp1 - temp3;
            wn1[is + js * wn1_dim1] = wn1[is + js * wn1_dim1] - temp2 + temp4;
            jpntr = jpntr % *m + 1;
            /* L40: */
        }
        ipntr = ipntr % *m + 1;
        /* L45: */
    }
    /*       modify the old parts in block (2,1). */
    ipntr = *head;
    i__1 = *m + upcl;
    for (is = *m + 1; is <= i__1; ++is) {
        jpntr = *head;
        i__2 = upcl;
        for (jy = 1; jy <= i__2; ++jy) {
            temp1 = 0.;
            temp3 = 0.;
            i__3 = *nenter;
            for (k = 1; k <= i__3; ++k) {
                k1 = indx2[k];
                temp1 += ws[k1 + ipntr * ws_dim1] * wy[k1 + jpntr * wy_dim1];
                /* L50: */
            }
            i__3 = *n;
            for (k = *ileave; k <= i__3; ++k) {
                k1 = indx2[k];
                temp3 += ws[k1 + ipntr * ws_dim1] * wy[k1 + jpntr * wy_dim1];
                /* L51: */
            }
            if (is <= jy + *m) {
                wn1[is + jy * wn1_dim1] = wn1[is + jy * wn1_dim1] + temp1 -
                                          temp3;
            } else {
                wn1[is + jy * wn1_dim1] = wn1[is + jy * wn1_dim1] - temp1 +
                                          temp3;
            }
            jpntr = jpntr % *m + 1;
            /* L55: */
        }
        ipntr = ipntr % *m + 1;
        /* L60: */
    }
    /*     Form the upper triangle of WN = [D+Y' ZZ'Y/theta   -L_a'+R_z' ] */
    /*                                     [-L_a +R_z        S'AA'S*theta] */
    m2 = *m << 1;
    i__1 = *col;
    for (iy = 1; iy <= i__1; ++iy) {
        is = *col + iy;
        is1 = *m + iy;
        i__2 = iy;
        for (jy = 1; jy <= i__2; ++jy) {
            js = *col + jy;
            js1 = *m + jy;
            wn[jy + iy * wn_dim1] = wn1[iy + jy * wn1_dim1] / *theta;
            wn[js + is * wn_dim1] = wn1[is1 + js1 * wn1_dim1] * *theta;
            /* L65: */
        }
        i__2 = iy - 1;
        for (jy = 1; jy <= i__2; ++jy) {
            wn[jy + is * wn_dim1] = -wn1[is1 + jy * wn1_dim1];
            /* L66: */
        }
        i__2 = *col;
        for (jy = iy; jy <= i__2; ++jy) {
            wn[jy + is * wn_dim1] = wn1[is1 + jy * wn1_dim1];
            /* L67: */
        }
        wn[iy + iy * wn_dim1] += sy[iy + iy * sy_dim1];
        /* L70: */
    }
    /*     Form the upper triangle of WN= [  LL'            L^-1(-L_a'+R_z')] */
    /*                                    [(-L_a +R_z)L'^-1   S'AA'S*theta  ] */
    /*        first Cholesky factor (1,1) block of wn to get LL' */
    /*                          with L' stored in the upper triangle of wn. */
    dpofa(&wn[wn_offset], &m2, col, info);
    if (*info != 0) {
        *info = -1;
        return 0;
    }
    /*        then form L^-1(-L_a'+R_z') in the (1,2) block. */
    col2 = *col << 1;
    i__1 = col2;
    for (js = *col + 1; js <= i__1; ++js) {
        dtrsl(&wn[wn_offset], &m2, col, &wn[js * wn_dim1 + 1], &c__11, info);
        /* L71: */
    }
    /*     Form S'AA'S*theta + (L^-1(-L_a'+R_z'))'L^-1(-L_a'+R_z') in the */
    /*        upper triangle of (2,2) block of wn. */
    i__1 = col2;
    for (is = *col + 1; is <= i__1; ++is) {
        i__2 = col2;
        for (js = is; js <= i__2; ++js) {
            wn[is + js * wn_dim1] += ddot(col, &wn[is * wn_dim1 + 1], &c__1,
                                          &wn[js * wn_dim1 + 1], &c__1);
            /* L74: */
        }
        /* L72: */
    }
    /*     Cholesky factorization of (2,2) block of wn. */
    dpofa(&wn[*col + 1 + (*col + 1) * wn_dim1], &m2, col, info);
    if (*info != 0) {
        *info = -2;
        return 0;
    }
    return 0;
}

int Lbfgs::formt(integer *m, double *wt, double *sy,
                 double *ss, integer *col, double *theta, integer *info)
{
    /* System generated locals */
    integer wt_dim1, wt_offset, sy_dim1, sy_offset, ss_dim1, ss_offset, i__1,
        i__2, i__3;

    /* Local variables */
    integer i__, j, k, k1;
    double ddum;

    /*     ************ */

    /*     Subroutine formt */

    /*       This subroutine forms the upper half of the pos. def. and symm. */
    /*         T = theta*SS + L*D^(-1)*L', stores T in the upper triangle */
    /*         of the array wt, and performs the Cholesky factorization of T */
    /*         to produce J*J', with J' stored in the upper triangle of wt. */

    /*     Subprograms called: */

    /*       Linpack ... dpofa. */


    /*                           *  *  * */

    /*     NEOS, November 1994. (Latest revision June 1996.) */
    /*     Optimization Technology Center. */
    /*     Argonne National Laboratory and Northwestern University. */
    /*     Written by */
    /*                        Ciyou Zhu */
    /*     in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. */


    /*     ************ */
    /*     Form the upper half of  T = theta*SS + L*D^(-1)*L', */
    /*        store T in the upper triangle of the array wt. */
    /* Parameter adjustments */
    ss_dim1 = *m;
    ss_offset = 1 + ss_dim1;
    ss -= ss_offset;
    sy_dim1 = *m;
    sy_offset = 1 + sy_dim1;
    sy -= sy_offset;
    wt_dim1 = *m;
    wt_offset = 1 + wt_dim1;
    wt -= wt_offset;

    /* Function Body */
    i__1 = *col;
    for (j = 1; j <= i__1; ++j) {
        wt[j * wt_dim1 + 1] = *theta * ss[j * ss_dim1 + 1];
        /* L52: */
    }
    i__1 = *col;
    for (i__ = 2; i__ <= i__1; ++i__) {
        i__2 = *col;
        for (j = i__; j <= i__2; ++j) {
            k1 = fmin(i__,j) - 1;
            ddum = 0.;
            i__3 = k1;
            for (k = 1; k <= i__3; ++k) {
                ddum += sy[i__ + k * sy_dim1] * sy[j + k * sy_dim1] / sy[k +
                                                                         k * sy_dim1];
                /* L53: */
            }
            wt[i__ + j * wt_dim1] = ddum + *theta * ss[i__ + j * ss_dim1];
            /* L54: */
        }
        /* L55: */
    }
    /*     Cholesky factorize T to J*J' with */
    /*        J' stored in the upper triangle of wt. */
    dpofa(&wt[wt_offset], m, col, info);
    if (*info != 0) {
        *info = -3;
    }
    return 0;
}

int Lbfgs::freev(integer *n, integer *nfree, integer *index,
                 integer *nenter, integer *ileave, integer *indx2, integer *iwhere,
                 logical *wrk, logical *updatd, logical *cnstnd, integer *iprint,
                 integer *iter)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, k, iact;


    /*     ************ */

    /*     Subroutine freev */

    /*     This subroutine counts the entering and leaving variables when */
    /*       iter > 0, and finds the index set of free and active variables */
    /*       at the GCP. */

    /*     cnstnd is a logical variable indicating whether bounds are present */

    /*     index is an integer array of dimension n */
    /*       for i=1,...,nfree, index(i) are the indices of free variables */
    /*       for i=nfree+1,...,n, index(i) are the indices of bound variables */
    /*       On entry after the first iteration, index gives */
    /*         the free variables at the previous iteration. */
    /*       On exit it gives the free variables based on the determination */
    /*         in cauchy using the array iwhere. */

    /*     indx2 is an integer array of dimension n */
    /*       On entry indx2 is unspecified. */
    /*       On exit with iter>0, indx2 indicates which variables */
    /*          have changed status since the previous iteration. */
    /*       For i= 1,...,nenter, indx2(i) have changed from bound to free. */
    /*       For i= ileave+1,...,n, indx2(i) have changed from free to bound. */


    /*                           *  *  * */

    /*     NEOS, November 1994. (Latest revision June 1996.) */
    /*     Optimization Technology Center. */
    /*     Argonne National Laboratory and Northwestern University. */
    /*     Written by */
    /*                        Ciyou Zhu */
    /*     in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. */


    /*     ************ */
    /* Parameter adjustments */
    --iwhere;
    --indx2;
    --index;

    /* Function Body */
    *nenter = 0;
    *ileave = *n + 1;
    if (*iter > 0 && *cnstnd) {
        /*                           count the entering and leaving variables. */
        i__1 = *nfree;
        for (i__ = 1; i__ <= i__1; ++i__) {
            k = index[i__];
            /*            write(6,*) ' k  = index(i) ', k */
            /*            write(6,*) ' index = ', i */
            if (iwhere[k] > 0) {
                --(*ileave);
                indx2[*ileave] = k;
                if (*iprint >= 100) {
                    printf("Variable %ld leaves the set of free variables\n", k );
                }
            }
            /* L20: */
        }
        i__1 = *n;
        for (i__ = *nfree + 1; i__ <= i__1; ++i__) {
            k = index[i__];
            if (iwhere[k] <= 0) {
                ++(*nenter);
                indx2[*nenter] = k;
                if (*iprint >= 100) {
                    printf("Variable %ld leaves the set of free variables\n", k );
                }
            }
            /* L22: */
        }
        if (*iprint >= 99) {
            i__1 = *n + 1 - *ileave;
            printf("%ld variables leave; %ld variables enter\n", i__1, *nenter );
        }
    }
    *wrk = *ileave < *n + 1 || *nenter > 0 || *updatd;
    /*     Find the index set of free and active variables at the GCP. */
    *nfree = 0;
    iact = *n + 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
        if (iwhere[i__] <= 0) {
            ++(*nfree);
            index[*nfree] = i__;
        } else {
            --iact;
            index[iact] = i__;
        }
        /* L24: */
    }
    if (*iprint >= 99) {
        i__1 = *iter + 1;
        printf("%ld variables are free at GCP iter %ld\n", *nfree, i__1 );
    }
    return 0;
}

int Lbfgs::hpsolb(integer *n, double *t, integer *iorder,
                  integer *iheap)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, j, k;
    double out, ddum;
    integer indxin, indxou;

    /*     ************ */

    /*     Subroutine hpsolb */

    /*     This subroutine sorts out the least element of t, and puts the */
    /*       remaining elements of t in a heap. */

    /*     n is an integer variable. */
    /*       On entry n is the dimension of the arrays t and iorder. */
    /*       On exit n is unchanged. */

    /*     t is a double precision array of dimension n. */
    /*       On entry t stores the elements to be sorted, */
    /*       On exit t(n) stores the least elements of t, and t(1) to t(n-1) */
    /*         stores the remaining elements in the form of a heap. */

    /*     iorder is an integer array of dimension n. */
    /*       On entry iorder(i) is the index of t(i). */
    /*       On exit iorder(i) is still the index of t(i), but iorder may be */
    /*         permuted in accordance with t. */

    /*     iheap is an integer variable specifying the task. */
    /*       On entry iheap should be set as follows: */
    /*         iheap .eq. 0 if t(1) to t(n) is not in the form of a heap, */
    /*         iheap .ne. 0 if otherwise. */
    /*       On exit iheap is unchanged. */


    /*     References: */
    /*       Algorithm 232 of CACM (J. W. J. Williams): HEAPSORT. */

    /*                           *  *  * */

    /*     NEOS, November 1994. (Latest revision June 1996.) */
    /*     Optimization Technology Center. */
    /*     Argonne National Laboratory and Northwestern University. */
    /*     Written by */
    /*                        Ciyou Zhu */
    /*     in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. */

    /*     ************ */
    /* Parameter adjustments */
    --iorder;
    --t;

    /* Function Body */
    if (*iheap == 0) {
        /*        Rearrange the elements t(1) to t(n) to form a heap. */
        i__1 = *n;
        for (k = 2; k <= i__1; ++k) {
            ddum = t[k];
            indxin = iorder[k];
            /*           Add ddum to the heap. */
            i__ = k;
        L10:
            if (i__ > 1) {
                j = i__ / 2;
                if (ddum < t[j]) {
                    t[i__] = t[j];
                    iorder[i__] = iorder[j];
                    i__ = j;
                    goto L10;
                }
            }
            t[i__] = ddum;
            iorder[i__] = indxin;
            /* L20: */
        }
    }
    /*     Assign to 'out' the value of t(1), the least member of the heap, */
    /*        and rearrange the remaining members to form a heap as */
    /*        elements 1 to n-1 of t. */
    if (*n > 1) {
        i__ = 1;
        out = t[1];
        indxou = iorder[1];
        ddum = t[*n];
        indxin = iorder[*n];
    /*        Restore the heap */
    L30:
        j = i__ + i__;
        if (j <= *n - 1) {
            if (t[j + 1] < t[j]) {
                ++j;
            }
            if (t[j] < ddum) {
                t[i__] = t[j];
                iorder[i__] = iorder[j];
                i__ = j;
                goto L30;
            }
        }
        t[i__] = ddum;
        iorder[i__] = indxin;
        /*     Put the least member in t(n). */
        t[*n] = out;
        iorder[*n] = indxou;
    }
    return 0;
}

int Lbfgs::matupd(integer *n, integer *m, double *ws,
                  double *wy, double *sy, double *ss, double *d__,
                  double *r__, integer *itail, integer *iupdat, integer *col,
                  integer *head, double *theta, double *rr, double *dr,
                  double *stp, double *dtd)
{
    /* System generated locals */
    integer ws_dim1, ws_offset, wy_dim1, wy_offset, sy_dim1, sy_offset,
        ss_dim1, ss_offset, i__1, i__2;

    /* Local variables */
    integer j;
    integer pointr;

    /*     ************ */

    /*     Subroutine matupd */

    /*       This subroutine updates matrices WS and WY, and forms the */
    /*         middle matrix in B. */

    /*     Subprograms called: */

    /*       Linpack ... dcopy, ddot. */


    /*                           *  *  * */

    /*     NEOS, November 1994. (Latest revision June 1996.) */
    /*     Optimization Technology Center. */
    /*     Argonne National Laboratory and Northwestern University. */
    /*     Written by */
    /*                        Ciyou Zhu */
    /*     in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. */


    /*     ************ */
    /*     Set pointers for matrices WS and WY. */
    /* Parameter adjustments */
    --r__;
    --d__;
    ss_dim1 = *m;
    ss_offset = 1 + ss_dim1;
    ss -= ss_offset;
    sy_dim1 = *m;
    sy_offset = 1 + sy_dim1;
    sy -= sy_offset;
    wy_dim1 = *n;
    wy_offset = 1 + wy_dim1;
    wy -= wy_offset;
    ws_dim1 = *n;
    ws_offset = 1 + ws_dim1;
    ws -= ws_offset;

    /* Function Body */
    if (*iupdat <= *m) {
        *col = *iupdat;
        *itail = (*head + *iupdat - 2) % *m + 1;
    } else {
        *itail = *itail % *m + 1;
        *head = *head % *m + 1;
    }
    /*     Update matrices WS and WY. */
    dcopy(n, &d__[1], &c__1, &ws[*itail * ws_dim1 + 1], &c__1);
    dcopy(n, &r__[1], &c__1, &wy[*itail * wy_dim1 + 1], &c__1);
    /*     Set theta=yy/ys. */
    *theta = *rr / *dr;
    /*     Form the middle matrix in B. */
    /*        update the upper triangle of SS, */
    /*                                         and the lower triangle of SY: */
    if (*iupdat > *m) {
        /*                              move old information */
        i__1 = *col - 1;
        for (j = 1; j <= i__1; ++j) {
            dcopy(&j, &ss[(j + 1) * ss_dim1 + 2], &c__1, &ss[j * ss_dim1 + 1]
                  , &c__1);
            i__2 = *col - j;
            dcopy(&i__2, &sy[j + 1 + (j + 1) * sy_dim1], &c__1, &sy[j + j *
                                                                            sy_dim1], &c__1);
            /* L50: */
        }
    }
    /*        add new information: the last row of SY */
    /*                                             and the last column of SS: */
    pointr = *head;
    i__1 = *col - 1;
    for (j = 1; j <= i__1; ++j) {
        sy[*col + j * sy_dim1] = ddot(n, &d__[1], &c__1, &wy[pointr *
                                                                 wy_dim1 + 1], &c__1);
        ss[j + *col * ss_dim1] = ddot(n, &ws[pointr * ws_dim1 + 1], &c__1, &
                                                                           d__[1], &c__1);
        pointr = pointr % *m + 1;
        /* L51: */
    }
    if (*stp == 1.) {
        ss[*col + *col * ss_dim1] = *dtd;
    } else {
        ss[*col + *col * ss_dim1] = *stp * *stp * *dtd;
    }
    sy[*col + *col * sy_dim1] = *dr;
    return 0;
}

int Lbfgs::projgr(integer *n, double *l, double *u,
                  integer *nbd, double *x, double *g, double *sbgnrm)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Local variables */
    integer i__;
    double gi;

    /*     ************ */

    /*     Subroutine projgr */

    /*     This subroutine computes the infinity norm of the projected */
    /*       gradient. */


    /*                           *  *  * */

    /*     NEOS, November 1994. (Latest revision June 1996.) */
    /*     Optimization Technology Center. */
    /*     Argonne National Laboratory and Northwestern University. */
    /*     Written by */
    /*                        Ciyou Zhu */
    /*     in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. */


    /*     ************ */
    /* Parameter adjustments */
    --g;
    --x;
    --nbd;
    --u;
    --l;

    /* Function Body */
    *sbgnrm = 0.;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
        gi = g[i__];
        if (nbd[i__] != 0) {
            if (gi < 0.) {
                if (nbd[i__] >= 2) {
                    /* Computing MAX */
                    d__1 = x[i__] - u[i__];
                    gi = fmax(d__1,gi);
                }
            } else {
                if (nbd[i__] <= 2) {
                    /* Computing MIN */
                    d__1 = x[i__] - l[i__];
                    gi = fmin(d__1,gi);
                }
            }
        }
        /* Computing MAX */
        d__1 = *sbgnrm, d__2 = fabs(gi);
        *sbgnrm = fmax(d__1,d__2);
        /* L15: */
    }
    return 0;
}

int Lbfgs::subsm(integer *n, integer *m, integer *nsub, integer *
                                                            ind, double *l, double *u, integer *nbd, double *x,
                 double *d__, double *xp, double *ws, double *wy,
                 double *theta, double *xx, double *gg, integer *col,
                 integer *head, integer *iword, double *wv, double *wn,
                 integer *iprint, integer *info)
{

    /* System generated locals */
    integer ws_dim1, ws_offset, wy_dim1, wy_offset, wn_dim1, wn_offset, i__1,
        i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, k, m2;
    double dk;
    integer js, jy;
    double xk;
    integer ibd, col2;
    double dd_p__, temp1, temp2, alpha;
    integer pointr;

    /*     ********************************************************************** */

    /*     This routine contains the major changes in the updated version. */
    /*     The changes are described in the accompanying paper */

    /*      Jose Luis Morales, Jorge Nocedal */
    /*      "Remark On Algorithm 788: L-BFGS-B: Fortran Subroutines for Large-Scale */
    /*       Bound Constrained Optimization". Decemmber 27, 2010. */

    /*             J.L. Morales  Departamento de Matematicas, */
    /*                           Instituto Tecnologico Autonomo de Mexico */
    /*                           Mexico D.F. */

    /*             J, Nocedal    Department of Electrical Engineering and */
    /*                           Computer Science. */
    /*                           Northwestern University. Evanston, IL. USA */

    /*                           January 17, 2011 */

    /*      ********************************************************************** */


    /*     Subroutine subsm */

    /*     Given xcp, l, u, r, an index set that specifies */
    /*       the active set at xcp, and an l-BFGS matrix B */
    /*       (in terms of WY, WS, SY, WT, head, col, and theta), */
    /*       this subroutine computes an approximate solution */
    /*       of the subspace problem */

    /*       (P)   min Q(x) = r'(x-xcp) + 1/2 (x-xcp)' B (x-xcp) */

    /*             subject to l<=x<=u */
    /*                       x_i=xcp_i for all i in A(xcp) */

    /*       along the subspace unconstrained Newton direction */

    /*          d = -(Z'BZ)^(-1) r. */

    /*       The formula for the Newton direction, given the L-BFGS matrix */
    /*       and the Sherman-Morrison formula, is */

    /*          d = (1/theta)r + (1/theta*2) Z'WK^(-1)W'Z r. */

    /*       where */
    /*                 K = [-D -Y'ZZ'Y/theta     L_a'-R_z'  ] */
    /*                     [L_a -R_z           theta*S'AA'S ] */

    /*     Note that this procedure for computing d differs */
    /*     from that described in [1]. One can show that the matrix K is */
    /*     equal to the matrix M^[-1]N in that paper. */

    /*     n is an integer variable. */
    /*       On entry n is the dimension of the problem. */
    /*       On exit n is unchanged. */

    /*     m is an integer variable. */
    /*       On entry m is the maximum number of variable metric corrections */
    /*         used to define the limited memory matrix. */
    /*       On exit m is unchanged. */

    /*     nsub is an integer variable. */
    /*       On entry nsub is the number of free variables. */
    /*       On exit nsub is unchanged. */

    /*     ind is an integer array of dimension nsub. */
    /*       On entry ind specifies the coordinate indices of free variables. */
    /*       On exit ind is unchanged. */

    /*     l is a double precision array of dimension n. */
    /*       On entry l is the lower bound of x. */
    /*       On exit l is unchanged. */

    /*     u is a double precision array of dimension n. */
    /*       On entry u is the upper bound of x. */
    /*       On exit u is unchanged. */

    /*     nbd is a integer array of dimension n. */
    /*       On entry nbd represents the type of bounds imposed on the */
    /*         variables, and must be specified as follows: */
    /*         nbd(i)=0 if x(i) is unbounded, */
    /*                1 if x(i) has only a lower bound, */
    /*                2 if x(i) has both lower and upper bounds, and */
    /*                3 if x(i) has only an upper bound. */
    /*       On exit nbd is unchanged. */

    /*     x is a double precision array of dimension n. */
    /*       On entry x specifies the Cauchy point xcp. */
    /*       On exit x(i) is the minimizer of Q over the subspace of */
    /*                                                        free variables. */

    /*     d is a double precision array of dimension n. */
    /*       On entry d is the reduced gradient of Q at xcp. */
    /*       On exit d is the Newton direction of Q. */

    /*    xp is a double precision array of dimension n. */
    /*       used to safeguard the projected Newton direction */

    /*    xx is a double precision array of dimension n */
    /*       On entry it holds the current iterate */
    /*       On output it is unchanged */
    /*    gg is a double precision array of dimension n */
    /*       On entry it holds the gradient at the current iterate */
    /*       On output it is unchanged */

    /*     ws and wy are double precision arrays; */
    /*     theta is a double precision variable; */
    /*     col is an integer variable; */
    /*     head is an integer variable. */
    /*       On entry they store the information defining the */
    /*                                          limited memory BFGS matrix: */
    /*         ws(n,m) stores S, a set of s-vectors; */
    /*         wy(n,m) stores Y, a set of y-vectors; */
    /*         theta is the scaling factor specifying B_0 = theta I; */
    /*         col is the number of variable metric corrections stored; */
    /*         head is the location of the 1st s- (or y-) vector in S (or Y). */
    /*       On exit they are unchanged. */

    /*     iword is an integer variable. */
    /*       On entry iword is unspecified. */
    /*       On exit iword specifies the status of the subspace solution. */
    /*         iword = 0 if the solution is in the box, */
    /*                 1 if some bound is encountered. */

    /*     wv is a double precision working array of dimension 2m. */

    /*     wn is a double precision array of dimension 2m x 2m. */
    /*       On entry the upper triangle of wn stores the LEL^T factorization */
    /*         of the indefinite matrix */

    /*              K = [-D -Y'ZZ'Y/theta     L_a'-R_z'  ] */
    /*                  [L_a -R_z           theta*S'AA'S ] */
    /*                                                    where E = [-I  0] */
    /*                                                              [ 0  I] */
    /*       On exit wn is unchanged. */

    /*     iprint is an INTEGER variable that must be set by the user. */
    /*       It controls the frequency and type of output generated: */
    /*        iprint<0    no output is generated; */
    /*        iprint=0    print only one line at the last iteration; */
    /*        0<iprint<99 print also f and |proj g| every iprint iterations; */
    /*        iprint=99   print details of every iteration except n-vectors; */
    /*        iprint=100  print also the changes of active set and final x; */
    /*        iprint>100  print details of every iteration including x and g; */
    /*       When iprint > 0, the file iterate.dat will be created to */
    /*                        summarize the iteration. */

    /*     info is an integer variable. */
    /*       On entry info is unspecified. */
    /*       On exit info = 0       for normal return, */
    /*                    = nonzero for abnormal return */
    /*                                  when the matrix K is ill-conditioned. */

    /*     Subprograms called: */

    /*       Linpack dtrsl. */


    /*     References: */

    /*       [1] R. H. Byrd, P. Lu, J. Nocedal and C. Zhu, ``A limited */
    /*       memory algorithm for bound constrained optimization'', */
    /*       SIAM J. Scientific Computing 16 (1995), no. 5, pp. 1190--1208. */



    /*                           *  *  * */

    /*     NEOS, November 1994. (Latest revision June 1996.) */
    /*     Optimization Technology Center. */
    /*     Argonne National Laboratory and Northwestern University. */
    /*     Written by */
    /*                        Ciyou Zhu */
    /*     in collaboration with R.H. Byrd, P. Lu-Chen and J. Nocedal. */


    /*     ************ */

    /* Parameter adjustments */
    --gg;
    --xx;
    --xp;
    --d__;
    --x;
    --nbd;
    --u;
    --l;
    wn_dim1 = 2 * *m;
    wn_offset = 1 + wn_dim1;
    wn -= wn_offset;
    --wv;
    wy_dim1 = *n;
    wy_offset = 1 + wy_dim1;
    wy -= wy_offset;
    ws_dim1 = *n;
    ws_offset = 1 + ws_dim1;
    ws -= ws_offset;
    --ind;

    /* Function Body */
    if (*nsub <= 0) {
        return 0;
    }
    if (*iprint >= 99) {
        printf("---------------SUBSM entered---------\n");
    }
    /*     Compute wv = W'Zd. */
    pointr = *head;
    i__1 = *col;
    for (i__ = 1; i__ <= i__1; ++i__) {
        temp1 = 0.;
        temp2 = 0.;
        i__2 = *nsub;
        for (j = 1; j <= i__2; ++j) {
            k = ind[j];
            temp1 += wy[k + pointr * wy_dim1] * d__[j];
            temp2 += ws[k + pointr * ws_dim1] * d__[j];
            /* L10: */
        }
        wv[i__] = temp1;
        wv[*col + i__] = *theta * temp2;
        pointr = pointr % *m + 1;
        /* L20: */
    }
    /*     Compute wv:=K^(-1)wv. */
    m2 = *m << 1;
    col2 = *col << 1;
    dtrsl(&wn[wn_offset], &m2, &col2, &wv[1], &c__11, info);
    if (*info != 0) {
        return 0;
    }
    i__1 = *col;
    for (i__ = 1; i__ <= i__1; ++i__) {
        wv[i__] = -wv[i__];
        /* L25: */
    }
    dtrsl(&wn[wn_offset], &m2, &col2, &wv[1], &c__1, info);
    if (*info != 0) {
        return 0;
    }
    /*     Compute d = (1/theta)d + (1/theta**2)Z'W wv. */
    pointr = *head;
    i__1 = *col;
    for (jy = 1; jy <= i__1; ++jy) {
        js = *col + jy;
        i__2 = *nsub;
        for (i__ = 1; i__ <= i__2; ++i__) {
            k = ind[i__];
            d__[i__] = d__[i__] + wy[k + pointr * wy_dim1] * wv[jy] / *theta
                       + ws[k + pointr * ws_dim1] * wv[js];
            /* L30: */
        }
        pointr = pointr % *m + 1;
        /* L40: */
    }
    d__1 = 1. / *theta;
    dscal(nsub, &d__1, &d__[1], &c__1);

    /* ----------------------------------------------------------------- */
    /*     Let us try the projection, d is the Newton direction */
    *iword = 0;
    dcopy(n, &x[1], &c__1, &xp[1], &c__1);

    i__1 = *nsub;
    for (i__ = 1; i__ <= i__1; ++i__) {
        k = ind[i__];
        dk = d__[i__];
        xk = x[k];
        if (nbd[k] != 0) {

            if (nbd[k] == 1) {
                /* lower bounds only */
                /* Computing MAX */
                d__1 = l[k], d__2 = xk + dk;
                x[k] = fmax(d__1,d__2);
                if (x[k] == l[k]) {
                    *iword = 1;
                }
            } else {

                if (nbd[k] == 2) {
                    /* upper and lower bounds */
                    /* Computing MAX */
                    d__1 = l[k], d__2 = xk + dk;
                    xk = fmax(d__1,d__2);
                    /* Computing MIN */
                    d__1 = u[k];
                    x[k] = fmin(d__1,xk);
                    if (x[k] == l[k] || x[k] == u[k]) {
                        *iword = 1;
                    }
                } else {

                    if (nbd[k] == 3) {
                        /* upper bounds only */
                        /* Computing MIN */
                        d__1 = u[k], d__2 = xk + dk;
                        x[k] = fmin(d__1,d__2);
                        if (x[k] == u[k]) {
                            *iword = 1;
                        }
                    }
                }
            }

        } else {
            /* free variables */
            x[k] = xk + dk;
        }
        /* L50: */
    }

    if (*iword == 0) {
        goto L911;
    }

    /*     check sign of the directional derivative */

    dd_p__ = 0.;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
        dd_p__ += (x[i__] - xx[i__]) * gg[i__];
        /* L55: */
    }
    if (dd_p__ > 0.) {
        dcopy(n, &xp[1], &c__1, &x[1], &c__1);

    } else {
        goto L911;
    }

    /* ----------------------------------------------------------------- */

    alpha = 1.;
    temp1 = alpha;
    ibd = 0;
    i__1 = *nsub;
    for (i__ = 1; i__ <= i__1; ++i__) {
        k = ind[i__];
        dk = d__[i__];
        if (nbd[k] != 0) {
            if (dk < 0. && nbd[k] <= 2) {
                temp2 = l[k] - x[k];
                if (temp2 >= 0.) {
                    temp1 = 0.;
                } else if (dk * alpha < temp2) {
                    temp1 = temp2 / dk;
                }
            } else if (dk > 0. && nbd[k] >= 2) {
                temp2 = u[k] - x[k];
                if (temp2 <= 0.) {
                    temp1 = 0.;
                } else if (dk * alpha > temp2) {
                    temp1 = temp2 / dk;
                }
            }
            if (temp1 < alpha) {
                alpha = temp1;
                ibd = i__;
            }
        }
        /* L60: */
    }
    if (alpha < 1.) {
        dk = d__[ibd];
        k = ind[ibd];
        if (dk > 0.) {
            x[k] = u[k];
            d__[ibd] = 0.;
        } else if (dk < 0.) {
            x[k] = l[k];
            d__[ibd] = 0.;
        }
    }
    i__1 = *nsub;
    for (i__ = 1; i__ <= i__1; ++i__) {
        k = ind[i__];
        x[k] += alpha * d__[i__];
        /* L70: */
    }
    /* ccccc */
L911:
    if (*iprint >= 99) {
        printf("----------------- exit SUBSM --------------\n");
    }
    return 0;
} /* subsm */



int Lbfgs::timer(double *ttime)
{
    clock_t temp;

    /*     this routine computes cpu time in double precision; it makes use of */
    /*     the intrinsic f90 cpu_time therefore a conversion type is */
    /*     needed. */

    /*           j.l morales  departamento de Matematicas, */
    /*                        instituto Tecnologico Autonomo de Mexico */
    /*                        mexico D.F. */

    /*           j.l nocedal  department of Electrical Engineering and */
    /*                        computer Science. */
    /*                        northwestern University. Evanston, IL. USA */

    /*                        january 21, 2011 */

    /*     temp = (real) (*ttime); */
    /*     *ttime = (double) temp; */
    temp    = clock();
    *ttime  = ((double) temp)/CLOCKS_PER_SEC;
    return 0;
} /* timer */
