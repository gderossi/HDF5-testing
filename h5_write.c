/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://www.hdfgroup.org/licenses.               *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 *  This example writes data to the HDF5 file.
 *  Data conversion is performed during write operation.
 */

#include "hdf5.h"
#include "windows.h"

#define H5FILE_NAME "D:/SDS.h5"
#define DATASETNAME "CharArray"
#define NX          1024 /* dataset dimensions */
#define NY          1024
#define NZ          102400
#define RANK        3

int
main(void)
{
    hid_t   file, dataset;                 /* file and dataset handles */
    hid_t   datatype, dataspace, memspace; /* handles */
    hsize_t dimsf[RANK];                   /* dataset dimensions */
    herr_t  status;
    char *  data; /* data to write */
    int     i;

    hsize_t count[3]  = {1, NY, NX};
    hsize_t offset[3] = {0, 0, 0};
    hsize_t stride[3] = {1, 1, 1};
    hsize_t block[3]  = {1, 1, 1};

    /*
     * Data  and output buffer initialization.
     */

    data = (char *)VirtualAlloc(NULL, NX * NY * sizeof(char), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    for (i = 0; i < NX * NY; i++)
        data[i] = (char)(i % 256);

    /*
     * Create a new file with Windows Direct I/O file driver
     */
    hid_t pList = H5Pcreate(H5P_FILE_ACCESS);
    //H5Pset_fapl_windirect(pList, MBOUNDARY_DEF, FBSIZE_DEF, CBSIZE_DEF);  //Custom file driver, comment this line out to use the default sec2 driver instead

    file = H5Fcreate(H5FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, pList);
    H5Pclose(pList);

    /*
     * Describe the size of the array and create the data space for fixed
     * size dataset.
     */
    dimsf[0]  = NZ;
    dimsf[1]  = NY;
    dimsf[2]  = NX;
    dataspace = H5Screate_simple(RANK, dimsf, dimsf);

    /*
     * Define datatype for the data in the file.
     */
    datatype = H5Tcopy(H5T_NATIVE_CHAR);
    status   = H5Tset_order(datatype, H5T_ORDER_LE); //Not sure this line actually does anything since I'm currently using chars...

    /*
     * Create a new dataset within the file using defined dataspace and
     * datatype and default dataset creation properties.
     */
    dataset = H5Dcreate2(file, DATASETNAME, datatype, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    H5Sclose(dataspace);

    memspace  = H5Screate_simple(RANK, count, NULL);
    dataspace = H5Dget_space(dataset);

    /*
     * Write the data to the dataset one plane at a time
     */
    for (i = 0; i < NZ; i++) {
        offset[0] = i;
        H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, stride, count, block);
        status = H5Dwrite(dataset, H5T_NATIVE_CHAR, memspace, dataspace, H5P_DEFAULT, data);
    }
    /*
     * Close/release resources.
     */
    H5Sclose(dataspace);
    H5Tclose(datatype);
    H5Dclose(dataset);
    H5Fclose(file);

    VirtualFree(data, 0, MEM_RELEASE);

    return 0;
}
