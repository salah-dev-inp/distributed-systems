echo "*************** makefile4 "
mpirun -machinefile $OAR_NODEFILE -mca mtl psm2 -mca pml ^ucx,ofi -mca btl ^ofi,openib ~/distributed-systems/parallel_exec_log ~/distributed-systems/makefiles-test/$1/makefile4 ~/distributed-systems/data/$1/$2 4
echo "*************** makefile8 "
mpirun -machinefile $OAR_NODEFILE -mca mtl psm2 -mca pml ^ucx,ofi -mca btl ^ofi,openib ~/distributed-systems/parallel_exec_log ~/distributed-systems/makefiles-test/$1/makefile8 ~/distributed-systems/data/$1/$2 8
echo "*************** makefile16"
mpirun -machinefile $OAR_NODEFILE -mca mtl psm2 -mca pml ^ucx,ofi -mca btl ^ofi,openib ~/distributed-systems/parallel_exec_log ~/distributed-systems/makefiles-test/$1/makefile16 ~/distributed-systems/data/$1/$2 16
echo "*************** makefile32"
mpirun -machinefile $OAR_NODEFILE -mca mtl psm2 -mca pml ^ucx,ofi -mca btl ^ofi,openib ~/distributed-systems/parallel_exec_log ~/distributed-systems/makefiles-test/$1/makefile32 ~/distributed-systems/data/$1/$2 32
echo "*************** makefile48"
mpirun -machinefile $OAR_NODEFILE -mca mtl psm2 -mca pml ^ucx,ofi -mca btl ^ofi,openib ~/distributed-systems/parallel_exec_log ~/distributed-systems/makefiles-test/$1/makefile48 ~/distributed-systems/data/$1/$2 48
echo "*************** makefile64"
mpirun -machinefile $OAR_NODEFILE -mca mtl psm2 -mca pml ^ucx,ofi -mca btl ^ofi,openib ~/distributed-systems/parallel_exec_log ~/distributed-systems/makefiles-test/$1/makefile64 ~/distributed-systems/data/$1/$2 64
echo "*************** makefile96"
mpirun -machinefile $OAR_NODEFILE -mca mtl psm2 -mca pml ^ucx,ofi -mca btl ^ofi,openib ~/distributed-systems/parallel_exec_log ~/distributed-systems/makefiles-test/$1/makefile96 ~/distributed-systems/data/$1/$2 96
echo "*************** makefile128"
mpirun -machinefile $OAR_NODEFILE -mca mtl psm2 -mca pml ^ucx,ofi -mca btl ^ofi,openib ~/distributed-systems/parallel_exec_log ~/distributed-systems/makefiles-test/$1/makefile128 ~/distributed-systems/data/$1/$2 128