#include <aws/core/Aws.h>
#include <aws/core/client/DefaultRetryStrategy.h>
#include <aws/core/utils/threading/Executor.h>
#include <aws/s3-crt/S3CrtClient.h>
#include <aws/s3-crt/model/PutObjectRequest.h>
#include <aws/s3/S3Client.h>
#include <aws/transfer/TransferManager.h>
#include <fstream>
#include <iostream>

// instance S3 client to test compilation and linking
int main(int argc, char* argv[])
{
    std::cout << "started" << std::endl;

    // esential othevice
    Aws::SDKOptions options;
    Aws::InitAPI(options);

    {
        std::cout << ">> s3 client started" << std::endl;
        auto executor =
            Aws::MakeShared<Aws::Utils::Threading::PooledThreadExecutor>("s3b-executor", 5);
        Aws::Client::ClientConfiguration configClient;
        configClient.endpointOverride = Aws::String("localhost");
        configClient.executor = executor;
        configClient.retryStrategy =
            Aws::MakeShared<Aws::Client::DefaultRetryStrategy>("s3b-client-retry", 1);

        auto client = Aws::MakeShared<Aws::S3::S3Client>("s3-client", configClient);

        Aws::Transfer::TransferManagerConfiguration transfer_config(executor.get());
        transfer_config.s3Client = client;
        transfer_config.transferBufferMaxHeapSize = Aws::Transfer::MB5 * 5;
        auto transferManager = Aws::Transfer::TransferManager::Create(transfer_config);

        auto uploadHandle = transferManager->UploadFile(
            "/dev/null",
            "bucketName",
            "objectPath",
            "application/octet-stream",
            Aws::Map<Aws::String, Aws::String>());
        uploadHandle->WaitUntilFinished();

        std::cout
            << (uploadHandle->GetStatus() == Aws::Transfer::TransferStatus::COMPLETED
                    ? "ok"
                    : uploadHandle->GetLastError().GetMessage().c_str())
            << std::endl;
        std::cout << ">> s3 client ended" << std::endl;
    }

    {
        std::cout << ">> s3-ctr client started" << std::endl;
        auto executor =
            Aws::MakeShared<Aws::Utils::Threading::PooledThreadExecutor>("s3b-executor", 5);

        Aws::S3Crt::ClientConfiguration configClient;
        configClient.endpointOverride = Aws::String("localhost");
        configClient.executor = executor;

        auto client = Aws::S3Crt::S3CrtClient{configClient};

        Aws::S3Crt::Model::PutObjectRequest request;
        request.SetBucket("bucketName");
        request.SetKey("objectKey");
        auto bodyStream = Aws::MakeShared<Aws::FStream>(
            "s3-file", "/dev/null", std::ios_base::in | std::ios_base::binary);
        if (!bodyStream->good()) {
            std::cout << "Failed to open file." << std::endl;
            return 1;
        }
        request.SetBody(bodyStream);

        // A PUT operation turns into a multipart upload using the s3-crt client.
        // https://github.com/aws/aws-sdk-cpp/wiki/Improving-S3-Throughput-with-AWS-SDK-for-CPP-v1.9
        Aws::S3Crt::Model::PutObjectOutcome outcome = client.PutObject(request);

        std::cout << (outcome.IsSuccess() ? "ok" : outcome.GetError().GetMessage().c_str())
                  << std::endl;
        std::cout << ">> s3-ctr client ended" << std::endl;
    }

    Aws::ShutdownAPI(options);
    std::cout << "ended" << std::endl;
    return 0;
}
