#include <iostream>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>

// instance S3 client to test compilation and linking
int main(int argc, char* argv[])
{
    std::cout << "started" << std::endl;

    // esential othevice
    Aws::SDKOptions options;
    Aws::InitAPI(options);

    Aws::S3::S3Client client;

    Aws::ShutdownAPI(options);
    std::cout << "ended" << std::endl;
    return 0;
}
