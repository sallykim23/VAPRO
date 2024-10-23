#include <iostream>
#include <cmath>
#include <cassert>
#include <random>
#include <chrono>
#include <fstream>
#include <unistd.h>
#include <string>
//enum communication
//{
//    Chain,
//};
const int MAX_LOOP_LEN=100000000;	// maximum of send_size
const int MAX_LOOPS=1000000;		// MAX LOOPS LIMIMT
int send_size=100000000;	// comm time
int loop_len=1;		// time of every calc()
int cnt_loops=100;		// real loop times
double calc_time=1;

int mpi_size,mpi_rank,mpi_size_per_node;
double start_time,start_time2,end_time;
struct Timestamp
{
    double before_calc, before_comm, before_PMPI, after_comm;
}local_time[MAX_LOOPS];
int cnt_iter;

double calc_result[MAX_LOOP_LEN];
double recv_buffer[MAX_LOOP_LEN];
std::minstd_rand rand_engine;

extern "C" void my_Init();
extern "C" void my_Barrier();
extern "C" double my_Wtime();
extern "C" void my_Finalize();


/*int my_Barrier(){
	printf("dummy barrier called\n");
	return 0;
}
double my_Wtime(){
	printf("dummy wtime called\n");
	return 0.0;
}
double my_Finalize(){
	printf("MPI finalized\n");
	return 0.0;
}*/


//int calc_op() //{
//    int x=1,y=2,z=3;
//    for (int i=0;i<100000;++i)
//    {
//        z=x*y;
//        x=y*z;
//        y=x*z;
//    }
//    return x+y+z;
//}

int calc_smallop() {
    int x=1,y=2,z=3;
    for (int i=0;i<100000;++i)
    {
        z=x*y;
        x=y*z;
        y=x*z;
                                    
    }
    return x+y+z;
}

int calc_op()// cost 0.6s
{
    int t;
    int sum=0;
    for (int i=0;i<20*calc_time/0.6;++i)
    {
        t=calc_smallop();
        sum+=t;
    }
    return sum;
}



void calc()
{
    calc_result[0]=0;
    // DEBUG: skewed overload
    static int cnt=0;
    ++cnt;
    int r = 0;
    if (mpi_rank < 128)
    {
        r = loop_len * 40 / (cnt / 10 + 4);
    } else
    {
        r = loop_len * 40 / (0 + 4);
    }
    for (int i = 0; i < r; ++i)
//        for (int i = 0; i < loop_len * 40 / (rand()%mpi_size + 4); ++i)
    {
        //calc_result[j]+=int(calc_result[j-1]+int(sin(sin(rand_engine())))*10000)%123123;
        calc_result[i] = calc_op();
    }
}

void calc2()
{
	auto start = std::chrono::high_resolution_clock::now();

	int R1 = 2000;
	int C1 = 2000;
	int R2 = 2000;
	int C2 = 2000;
    int* mat1 = (int*)malloc(sizeof(int)*R1*C1);
    int* mat2 = (int*)malloc(sizeof(int)*R2*C2);
    int* comb = (int*)malloc(sizeof(int)*R1*C2);
	for (int i = 0; i < R1; i++) {
		for (int j = 0; j < C1; j++) {
			mat1[i*C1+j] = (int)(100 * sin(i + j) + 5 * pow(i, j));
		}
	}
	for (int i = 0; i < R2; i++) {
		for (int j = 0; j < C2; j++) {
			mat2[i*C2+j] = (int)(200 * cos(i - j) + 3 * (i + j) * (i - j));
		}
	}

    for (int i=0;i< R1;i++){
        for(int j=0; j< C2;j++){
            comb[i*C2+j] =0;
        }
    }

    for (int kk = 0; kk < 1; kk++) {
	for (int i=0;i< R1;i++){
		for(int j=0; j< C2;j++){
			for(int k=0; k<R2;k++){
				comb[i*C2+j] += mat1[i*C1+k] * mat2[k*C2+j];
			}
		}
	}
    }

    free(mat1);
    free(mat2);
    free(comb);

    auto stop = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
}
/*
void ioWork(int np){
	auto start = std::chrono::high_resolution_clock::now();
	std::ofstream myfile;
	std::string filename = std::to_string(np) + ".txt";
	myfile.open(filename);
	int count = 1;	
	
	
	while(count <=100){{
		sleep(1);
		if (myfile.is_open()){		
			myfile << "Hi\n";
			myfile << "This is another line\n";
		}
		count ++;
	}
	



	myfile.close();

    	auto stop = std::chrono::high_resolution_clock::now();
    	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "IO workload " << std::endl;
	std::cout << time.count() << std::endl;
}*/
/*
void ioWork(int np){
	auto start = std::chrono::high_resolution_clock::now();
	std::ofstream myfile;
	std::string filename = std::to_string(np) + ".txt";
	myfile.open(filename);
	int count =1;		
	
	while(count<= 100){
		sleep(1);
		if (myfile.is_open()){
			char asciiVal = static_cast<char>(count+32);		
			myfile << asciiVal << std::endl;
		}
		count++;
	}
	myfile.close();


	std::ifstream text(filename);
	std::string filename2 = std::to_string(np) + "out.txt";
	std::ofstream outcome(filename2);
	

	if(text.is_open() && outcome.is_open()){
		char ch;
		while (text.get(ch)){
			int asciiVal = static_cast<int>(ch);
			outcome << asciiVal << std::endl;
		}
	}

	text.close();
	outcome.close();

    	auto stop = std::chrono::high_resolution_clock::now();
    	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "IO workload " << std::endl;
	std::cout << time.count() << std::endl;
}*/




/*
void comm(int iter)
{
    cnt_iter=iter;
//    fprintf(stderr, "rank=%d target rank=%d\n", mpi_rank, mpi_rank + mpi_size_per_node);
#ifdef SEND_RECV
    if (mpi_rank<mpi_size_per_node)
    {
        MPI_Send(calc_result, send_size, MPI_DOUBLE, mpi_rank + mpi_size_per_node, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Status status;
        MPI_Recv(recv_buffer, send_size, MPI_DOUBLE, mpi_rank - mpi_size_per_node, 0, MPI_COMM_WORLD, &status);
    }
#else // ALL_REDUCE
    MPI_Allreduce(calc_result, recv_buffer, send_size, MPI_DOUBLE, MPI_SUM,
                  MPI_COMM_WORLD);
#endif
}*/
/*
void ioWork(int np){
	MPI_File fh;
	MPI_File_open(MPI_COMM_WORLD, "output.txt", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
	int*buffer = (int *)malloc(100*sizeof(int));
	for(int i=0;i<100;i++){
		buffer[i] = i;
	}
	MPI_File_write_at(fh, np * sizeof(int), buffer, 1, MPI_INT, MPI_STATUS_IGNORE);
	
	MPI_File_close(&fh);
}*/
int main(int argc, char* argv[])
{
    if (argc!=4)
    {
        fprintf(stderr,"argc != 4");
        return -1;
    }
    my_Init();
    my_Barrier();

    send_size=atoi(argv[1]);
    //double send_time=atof(argv[1]);
    //loop_len=atoi(argv[2]);
    calc_time=atof(argv[2]);
    cnt_loops=atoi(argv[3]);
    //MPI_Comm_rank(MPI_COMM_WORLD,&mpi_rank);
    //mpi_size_per_node=mpi_size/2;
    //MPI_Barrier(MPI_COMM_WORLD);
    start_time=my_Wtime();
    my_Barrier();

    for (int i=0;i<cnt_loops;++i)
    {
//    fprintf(stderr,"start calc %d\n",i);//DEBUG
        local_time[i].before_calc=my_Wtime();
        calc();
        //MPI_Barrier(MPI_COMM_WORLD);
        //local_time[i].before_comm=MPI_Wtime();
        //calc2();
        //MPI_Barrier(MPI_COMM_WORLD);
        local_time[i].before_comm=my_Wtime();
        //ioWork(mpi_rank);
        //MPI_Barrier(MPI_COMM_WORLD);
        //local_time[i].before_comm=MPI_Wtime();
        //comm(i);
        //MPI_Barrier(MPI_COMM_WORLD);
        local_time[i].after_comm=my_Wtime();
//    fprintf(stderr,"finish calc %d\n",i);//DEBUG
    }

    my_Barrier();
    end_time=my_Wtime();
/*
    MPI_Datatype mpi_timestamp;
    int length[1]={4};
    MPI_Aint disp[1]={0};
    MPI_Datatype types[1]={MPI_DOUBLE};
    MPI_Type_create_struct(1, length, disp, types, &mpi_timestamp);
    MPI_Type_commit(&mpi_timestamp);
    if (mpi_rank==0)
    {
        fprintf(stderr, "Wall time: %lf\n", end_time - start_time);
        //fprintf(stdout, "Wall time: %lf\n", end_time - start_time);
        fprintf(stderr, "3~Wall time: %lf\n", end_time - local_time[2].before_calc);
    }*/
    my_Finalize();
    //fprintf(stderr, "Rank %d finalized.\n", mpi_rank);
    return 0;
}
