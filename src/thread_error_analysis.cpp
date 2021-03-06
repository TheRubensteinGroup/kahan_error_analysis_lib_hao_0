#include "thread_error_analysis.h"

using namespace std;

void calculate_mean_err_between_thread(complex<double> value_thread, complex<double>& mean, double& err)
{

#ifdef MPI_HAO
    int size; MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Calculate mean, send to all the tread
    MPI_Allreduce(&value_thread, &mean, 1, MPI_DOUBLE_COMPLEX, MPI_SUM, MPI_COMM_WORLD);
    mean/= (size*1.0);

    //Calculate err, only send to main thread
    if(size!=1)
    {
        double diff_thread=norm(value_thread-mean);
        MPI_Reduce(&diff_thread, &err, 1 , MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        err = sqrt( err / ( size*(size-1.0) ) );
    }
    else
    {
        err = 0.0;
    }
#else
    mean=value_thread; err=0.0;
#endif
}


void calculate_mean_err_between_thread(int N, const complex<double>* value_thread, complex<double>* mean, double* err)
{
#ifdef MPI_HAO
    int size; MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Calculate mean, send to all the tread
    MPI_Allreduce(value_thread, mean, N, MPI_DOUBLE_COMPLEX, MPI_SUM, MPI_COMM_WORLD); 
    for(int i=0; i<N; i++) mean[i]/=(size*1.0);

    //Calculate err, only send to main thread
    if(size!=1)
    {
        double* diff_thread = new double[N]; 
        for(int i=0; i<N; i++) diff_thread[i]=norm( value_thread[i]-mean[i] );
        MPI_Reduce(diff_thread, err, N , MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        for(int i=0; i<N; i++) err[i]=sqrt( err[i]/( size*(size-1.0) ) );
        delete[] diff_thread;
    }
    else
    {
        for(int i=0; i<N; i++) err[i]=0.0;
    }
#else
    for(int i=0; i<N; i++) {mean[i]=value_thread[i]; err[i]=0.0;}
#endif
}


void calculate_mean_err_between_thread(const vector< complex<double> >& value_thread,
                                             vector< complex<double> >& mean, vector<double>& err)
{
    int N_value=value_thread.size(); int N_mean=mean.size(); int N_err=err.size();
    if(N_value!=N_mean || N_value!=N_err) 
    {
        cout<<"Something is wrong with calculate_mean_err_between_thread vector input!"<<endl;
        exit(1);
    }
    calculate_mean_err_between_thread( N_value, value_thread.data(), mean.data(), err.data() );
}
