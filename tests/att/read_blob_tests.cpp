#define BOOST_TEST_MODULE
#include <boost/test/included/unit_test.hpp>

#include "test_servers.hpp"

BOOST_AUTO_TEST_SUITE( read_blob_errors )

BOOST_FIXTURE_TEST_CASE( pdu_to_small, small_temperature_service_with_response<> )
{
    BOOST_CHECK( check_error_response( { 0x0C, 0x02, 0x00, 0x00 }, 0x0C, 0x0000, 0x04 ) );
}

BOOST_FIXTURE_TEST_CASE( pdu_to_large, small_temperature_service_with_response<> )
{
    BOOST_CHECK( check_error_response( { 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00 }, 0x0C, 0x0000, 0x04 ) );
}

BOOST_FIXTURE_TEST_CASE( no_such_handle, small_temperature_service_with_response<> )
{
    BOOST_CHECK( check_error_response( { 0x0C, 0x17, 0xAA, 0x00, 0x00 }, 0x0C, 0xAA17, 0x0A ) );
    BOOST_CHECK( check_error_response( { 0x0C, 0x04, 0x00, 0x00, 0x00 }, 0x0C, 0x0004, 0x0A ) );
}

BOOST_FIXTURE_TEST_CASE( invalid_handle, small_temperature_service_with_response<> )
{
    BOOST_CHECK( check_error_response( { 0x0C, 0x00, 0x00, 0x00, 0x00 }, 0x0C, 0x0000, 0x01 ) );
}

char blob[ 100 ] = {};

typedef bluetoe::server<
    bluetoe::service<
        bluetoe::service_uuid< 0x8C8B4094, 0x0DE2, 0x499F, 0xA28A, 0x4EED5BC73CA9 >,
        bluetoe::characteristic<
            bluetoe::characteristic_uuid< 0x8C8B4094, 0x0DE2, 0x499F, 0xA28A, 0x4EED5BC73CAA >,
            bluetoe::bind_characteristic_value< decltype( blob ), &blob >,
            bluetoe::no_read_access
        >
    >
> unreadable_blob_server;


BOOST_FIXTURE_TEST_CASE( not_readable, request_with_reponse< unreadable_blob_server > )
{
    BOOST_CHECK( check_error_response( { 0x0C, 0x03, 0x00, 0x00, 0x00 }, 0x0C, 0x0003, 0x02 ) );
}

BOOST_FIXTURE_TEST_CASE( read_behind_end, small_temperature_service_with_response<> )
{
    BOOST_CHECK( check_error_response( { 0x0C, 0x03, 0x00, 0x05, 0x00 }, 0x0C, 0x0003, 0x07 ) );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( read_blob )

static const char const_blob[ 50 ] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49
};

typedef bluetoe::server<
    bluetoe::service<
        bluetoe::service_uuid< 0x8C8B4094, 0x0DE2, 0x499F, 0xA28A, 0x4EED5BC73CA9 >,
        bluetoe::characteristic<
            bluetoe::characteristic_uuid< 0x8C8B4094, 0x0DE2, 0x499F, 0xA28A, 0x4EED5BC73CAA >,
            bluetoe::bind_characteristic_value< decltype( const_blob ), &const_blob >
        >
    >
> blob_server;

BOOST_FIXTURE_TEST_CASE( read_starting_at_0, request_with_reponse< blob_server > )
{
    l2cap_input( { 0x0C, 0x03, 0x00, 0x00, 0x00 } );
    expected_result( {
        0x0D,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
        0x20, 0x21
    } );
}

BOOST_FIXTURE_TEST_CASE( read_starting_at_50, request_with_reponse< blob_server > )
{
    l2cap_input( { 0x0C, 0x03, 0x00, 0x32, 0x00 } );
    expected_result( { 0x0D } );
}

BOOST_FIXTURE_TEST_CASE( read_starting_at_10, request_with_reponse< blob_server > )
{
    l2cap_input( { 0x0C, 0x03, 0x00, 0x0A, 0x00 } );
    expected_result( {
        0x0D,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
        0x30, 0x31
    } );
}

BOOST_AUTO_TEST_SUITE_END()