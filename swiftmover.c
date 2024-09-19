#include <Windows.h>

#include "../include/swiftheader.h"

#define RELEASE_BCPM bCpManager->lpVtbl->Release(bCpManager);
#define RELEASE_BCPJ bCpJob->lpVtbl->Release(bCpJob);

#define BG_SERVER_FILE_MAX_LEN 2199
#define BG_LOCAL_FILE_MAX_LEN MAX_PATH

// C:\WINDOWS\System32\svchost.exe -k netsvcs -p (BITS)
LPWSTR returnMsgBuffer(DWORD errorCode);

int main(void)
{
    HRESULT hResult;

    /*
        Creates transfer jobs, retrieves an enumerator object that contains the 
        jobs in the queue, and retrieves individual jobs from the queue.
    */
    IBackgroundCopyManager *bCpManager = NULL;
    
    /*
        interface to add files to the job, set the priority level of the job
        determine the state of the job, and to start and stop the job.
    */
    IBackgroundCopyJob *bCpJob = NULL;

    // init COM (Component Object Model) limit to one thread
    hResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    if (FAILED(hResult))
    {
        printf("Failed to initialize COM library. Error code: 0x%08X\n", hResult);
        CoUninitialize();
        return -1;
    }

    hResult = CoCreateInstance
    (
        &CLSID_BackgroundCopyManager, 
        NULL, 
        CLSCTX_LOCAL_SERVER,
        &IID_IBackgroundCopyManager, (void**)&bCpManager
    );

    if (FAILED(hResult))
    {
        printf("Failed to start the BITS manager instance. Error code: 0x%08X\n", hResult);
        CoUninitialize();
        return -1;
    }

    // create the BITS job container that will hold files for transfer
    hResult = bCpManager->lpVtbl->CreateJob
    (
        bCpManager, 
        L"Test BITS Job", 
        BG_JOB_TYPE_DOWNLOAD,
        NULL,
        &bCpJob
    );

    switch (hResult)
    {
        case S_OK:
            printf("++ Successfully generated the job\n");
            break;
        
        case E_INVALIDARG:
            printf("-- The display name is too long for the specified job\n");
            
            RELEASE_BCPM
            CoUninitialize();

            return -1;
        
        case BG_E_TOO_MANY_JOBS_PER_MACHINE:
            printf("-- MaxJobsPerMachine from Group policy setting exceeded!\n");
            
            RELEASE_BCPM
            CoUninitialize();

            return -1;
        
        case BG_E_TOO_MANY_JOBS_PER_USER:
            printf("-- MaxJobsPerUser from Group policy setting exceeded!\n");
            
            RELEASE_BCPM
            CoUninitialize();

            return -1;
    }

    printf("++ JOB CREATED SUCCESSFULLY!\n");

    // create BG_FILE_INFO struct
    BG_FILE_INFO fileTransfer;

    LPWSTR remoteFile = L"https://raw.githubusercontent.com/PlatinumVoyager/fwstatus/main/current_proc_admin.c\0";
    LPWSTR localFile = L"C:\\Users\\DUPPY0\\Documents\\Development\\C\\Projects\\SwiftMover\\admin.c\0";

    memset(&fileTransfer, 0, sizeof(BG_FILE_INFO));

    fileTransfer.RemoteName = remoteFile;
    fileTransfer.LocalName =  localFile;

    wprintf(L"REMOTE=\"%s\", LOCAL=\"%s\"\n", remoteFile, localFile);
    wprintf(L"ERROR: %s\n", returnMsgBuffer(GetLastError()));

    // include files to the job container
    hResult = bCpJob->lpVtbl->AddFileSet(
        bCpJob,
        1,
        &fileTransfer
    );

    printf("HERE!\n");

    if (FAILED(hResult))
    {
        printf("Failed to add file to active BITS job container. Error code: 0x%08X\n", hResult);

        RELEASE_BCPJ
        RELEASE_BCPM

        CoUninitialize();

        return -1;
    }

    printf("++ Added file to job successfully!\n");

    // start the Job
    hResult = bCpJob->lpVtbl->Resume(bCpJob);

    if (FAILED(hResult))
    {
        printf("Failed to start the BITS job. Error Code: 0x%08X\n", hResult);

        RELEASE_BCPJ
        RELEASE_BCPM

        CoUninitialize();

        return -1;
    }

    // wait for the job to complete
    BG_JOB_STATE bJobState;

    do 
    {
        Sleep(1000);

        hResult = bCpJob->lpVtbl->GetState(bCpJob, &bJobState);

        if (FAILED(hResult))
        {
            printf("Failed to get the job state. Error Code: 0x%08X\n", hResult);

            RELEASE_BCPJ
            RELEASE_BCPM

            CoUninitialize();

            return -1;
        }
    } while (
        bJobState != BG_JOB_STATE_TRANSFERRED 
        && bJobState != BG_JOB_STATE_CANCELLED
        && bJobState != BG_JOB_STATE_ERROR
    );

    switch (bJobState)
    {
        case BG_JOB_STATE_ERROR:
        {
            IBackgroundCopyError *bCpError = NULL;
            hResult = bCpJob->lpVtbl->GetError(bCpJob, &bCpError);
        
            printf("BITS file transfer error. Error code: 0x%08X\n", hResult);
            bCpError->lpVtbl->Release(bCpError);

            RELEASE_BCPJ
            RELEASE_BCPM

            CoUninitialize();

            return -1;
        }

        case BG_JOB_STATE_CANCELLED:
        {
            printf("The BITS job has been cancelled. All files are successfully deleted.\n");

            break;
        }
    }

    // free
    RELEASE_BCPJ
    RELEASE_BCPM

    CoUninitialize();

    printf("++ FREED and FINISHED. Success...\n");

    return 0;
}


LPWSTR returnMsgBuffer(DWORD errorCode)
{
    LPWSTR msgBuffer = NULL;

    DWORD result = FormatMessageW
    (
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&msgBuffer,
        0,
        NULL
    );

    LocalFree(msgBuffer);

    return msgBuffer;
}
