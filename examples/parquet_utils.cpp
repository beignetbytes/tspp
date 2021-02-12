
#include "parquet_utils.hpp"
#include <tspp/time_series.hpp>
//#include <arrow/api.h>
#include <arrow/io/api.h>

#include "utilities.hpp"
#include <iomanip>
#include <parquet/arrow/reader.h>
#include <parquet/exception.h>
#include <parquet/stream_writer.h>

//#include <cstdint>
//#include <iostream>
//#include <vector>
#include <string>

void parquet_utils::time_series_to_file(const std::string &file_name, std::shared_ptr<tspp::time_series<double>> ts) {

	// std::shared_ptr<arrow::Table> table = parquet_utils::generate_table(ts);
	// parquet_utils::write_parquet_file(file_name, *table);

	std::shared_ptr<arrow::io::FileOutputStream> outfile;

	PARQUET_ASSIGN_OR_THROW(outfile, arrow::io::FileOutputStream::Open(file_name));

	parquet::WriterProperties::Builder builder;
	builder.version(parquet::ParquetVersion::PARQUET_2_0);

	std::shared_ptr<parquet::WriterProperties> props = builder.compression(parquet::Compression::type::SNAPPY)->build();

	// std::shared_ptr<parquet::schema::GroupNode> schema;
	parquet::schema::NodePtr root;
	parquet::schema::NodeVector fields;

	fields.push_back(parquet::schema::PrimitiveNode::Make("t", parquet::Repetition::REQUIRED, parquet::Type::INT64, parquet::ConvertedType::TIMESTAMP_MILLIS));
	fields.push_back(parquet::schema::PrimitiveNode::Make("value", parquet::Repetition::REQUIRED, parquet::Type::DOUBLE, parquet::ConvertedType::NONE));

	root = parquet::schema::GroupNode::Make("schema", parquet::Repetition::REQUIRED, fields);
	parquet::SchemaDescriptor schema;
	schema.Init(root);

	std::shared_ptr<parquet::schema::GroupNode> nodes = std::static_pointer_cast<parquet::schema::GroupNode>(root);

	// Set up builder with required compression type etc.
	// Define schema.
	// ...
	parquet::StreamWriter os{parquet::ParquetFileWriter::Open(outfile, nodes, props)};
	os.SetMaxRowGroupSize(1000);

	for (auto il = ts->begin(); il != ts->end(); ++il) {

		auto duration = ((*il).first).time_since_epoch();
		std::chrono::milliseconds dt = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		os << dt << (*il).second << parquet::EndRow;
	}
};

arrow::Status parquet_utils::time_series_from_file(const std::string &file_name, std::shared_ptr<tspp::time_series<double>> ts) {

	std::shared_ptr<arrow::io::ReadableFile> infile;
	PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(file_name, arrow::default_memory_pool()));

	std::unique_ptr<parquet::arrow::FileReader> reader;
	PARQUET_THROW_NOT_OK(parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader));
	std::shared_ptr<arrow::Table> table;
	PARQUET_THROW_NOT_OK(reader->ReadTable(&table));

	// TODO replace this part with a generic schema provider and constructor
	std::vector<std::shared_ptr<arrow::Field>> schema_vector = {
		arrow::field("t", arrow::timestamp(arrow::TimeUnit::type::MILLI)),
		arrow::field("value", arrow::float64()),
	};
	auto expected_schema = std::make_shared<arrow::Schema>(schema_vector);

	if (!expected_schema->Equals(*table->schema())) {
		// The table doesn't have the expected schema thus we cannot directly
		// convert it to our target representation.
		return arrow::Status::Invalid("Schemas are not matching!");
	}

	auto timestamps = std::static_pointer_cast<arrow::TimestampArray>(table->column(0)->chunk(0));
	auto values = std::static_pointer_cast<arrow::DoubleArray>(table->column(1)->chunk(0));

	for (int64_t i = 0; i < table->num_rows(); i++) {

		// int64_t t_pre = timestamps->Value(i)/(double)1000.0; //ugh seconds
		int64_t t_pre = timestamps->Value(i); // / (double)1000.0; // ugh seconds
		std::chrono::milliseconds dt(t_pre);

		std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::time_point(dt);

		/*boost::posix_time::ptime t(boost::posix_time::from_time_t(0).date(), boost::posix_time::milliseconds(dt.count()));*/

		double value = values->Value(i);

		// boost::posix_time::ptime t = boost::posix_time::from_time_t(t_pre);
		ts->insert_end(std::make_pair(t, value));
	}

	return arrow::Status::OK();
};
