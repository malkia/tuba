#include <stdio.h>

#include "grpcpp/grpcpp.h"

#include "opentelemetry/proto/collector/logs/v1/logs_service.pb.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
#include "opentelemetry/proto/collector/metrics/v1/metrics_service.pb.h"

#include "opentelemetry/proto/collector/logs/v1/logs_service.grpc.pb.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.h"
#include "opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.h"

extern "C"
#ifdef _WIN32
 __declspec(dllexport) 
#endif
void metrics_test()
{
	using namespace opentelemetry::proto::collector::metrics::v1;
	using namespace opentelemetry::proto::metrics::v1;

	NumberDataPoint numberDataPoint;

	const auto now{ time(nullptr) * 1000000000LL };

	numberDataPoint.set_start_time_unix_nano( now );
	numberDataPoint.set_time_unix_nano( now );
	numberDataPoint.set_as_double( 3.14159 );

	// Not yet supported by our grafana/prometheus, but available in OTEL
	Exemplar exemplar;
	exemplar.set_time_unix_nano( now );
	exemplar.set_as_double( 2.15 );

	// And here, just add one exemplar
	*numberDataPoint.add_exemplars() = exemplar;

	Metric metric;
	metric.set_name( "malkia_metric_grpc_test" ); // look for this in grafana
	metric.set_description( "malkia is testing grpc otel endpoint" );
	metric.set_unit("s"); // seconds - check http://unitsofmeasure.org/ucum.html

	// Just add one number data point
	*metric.mutable_gauge()->add_data_points() = numberDataPoint;

	ScopeMetrics scopeMetrics;
	*scopeMetrics.add_metrics() = metric;

	ResourceMetrics resourceMetrics;
	*resourceMetrics.add_scope_metrics() = scopeMetrics;

	ExportMetricsServiceRequest request;
	*request.add_resource_metrics() = resourceMetrics;

	ExportMetricsServiceResponse response;

	::grpc::ClientContext clientContext;
	const auto channel{ ::grpc::CreateChannel("localhost:50051", ::grpc::InsecureChannelCredentials()) };
	const auto metricsService{ MetricsService::NewStub(channel) };
	const auto status{ metricsService->Export(&clientContext, request, &response ) };
	if( !status.ok() )
	{
		printf( "error code:    %d\n", status.error_code() );
		printf( "error message: %s\n", status.error_message().c_str() );
		printf( "error details: %s\n", status.error_details().c_str() );
	}
	else
	{
		// https://github.com/open-telemetry/opentelemetry-proto/blob/main/opentelemetry/proto/collector/metrics/v1/metrics_service.proto#L45
		printf( "OK. Printing response:\n");
		printf( "  partial_success.rejected_data_points = %zd\n", response.partial_success().rejected_data_points() );
		printf( "  error_message = [%s]\n", response.partial_success().error_message().c_str() );
	}
}

int main()
{
	metrics_test();
	return 0;
}
