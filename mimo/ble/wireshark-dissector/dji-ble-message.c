#include <epan/packet.h>

WS_DLL_PUBLIC_DEF const char plugin_version[] = "0.0.1";
WS_DLL_PUBLIC_DEF const int plugin_want_major = WIRESHARK_VERSION_MAJOR;
WS_DLL_PUBLIC_DEF const int plugin_want_minor = WIRESHARK_VERSION_MINOR;

static int proto_dji_mimo_ble;

static dissector_handle_t dji_mimo_ble_handle;

static int ett_dji_mimo_ble = -1;
static int hf_dji_mimo_ble_magic;
static int hf_dji_mimo_ble_length;
static int hf_dji_mimo_ble_protocol_version;
static int hf_dji_mimo_ble_crc8_header;
static int hf_dji_mimo_ble_subsystem;
static int hf_dji_mimo_ble_message_id;
static int hf_dji_mimo_ble_message_type;
static int hf_dji_mimo_ble_payload;
static int hf_dji_mimo_ble_crc16_msg;

static int dissect_dji_mimo_ble(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree _U_, void *data _U_)
{
    int offset = 0;
    proto_item *ti;
    proto_tree *st;

    if (tvb_get_guint8(tvb, 0) != 0x55) {
        return 0;
    }

    ti = proto_tree_add_item(tree, proto_dji_mimo_ble, tvb, 0, -1, ENC_NA);
    st = proto_item_add_subtree(ti, ett_dji_mimo_ble);

    proto_tree_add_item(st, hf_dji_mimo_ble_magic, tvb, offset, 1, ENC_LITTLE_ENDIAN);
    offset += 1;
    proto_tree_add_item(st, hf_dji_mimo_ble_length, tvb, offset, 1, ENC_LITTLE_ENDIAN);
    uint8_t length = tvb_get_guint8(tvb, offset);
    offset += 1;
    proto_tree_add_item(st, hf_dji_mimo_ble_protocol_version, tvb, offset, 1, ENC_LITTLE_ENDIAN);
    offset += 1;
    proto_tree_add_item(st, hf_dji_mimo_ble_crc8_header, tvb, offset, 1, ENC_LITTLE_ENDIAN);
    offset += 1;
    proto_tree_add_item(st, hf_dji_mimo_ble_subsystem, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset += 2;
    proto_tree_add_item(st, hf_dji_mimo_ble_message_id, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset += 2;
    proto_tree_add_item(st, hf_dji_mimo_ble_message_type, tvb, offset, 3, ENC_BIG_ENDIAN);
    offset += 3;
    proto_tree_add_item(st, hf_dji_mimo_ble_payload, tvb, offset, length-13, ENC_LITTLE_ENDIAN);
    offset += length-13;
    proto_tree_add_item(st, hf_dji_mimo_ble_crc16_msg, tvb, offset, 2, ENC_LITTLE_ENDIAN);
    offset += 2;

    return offset;
}

static void proto_register_dji_mimo_ble(void)
{
    static hf_register_info hf[] = {
        {
            &hf_dji_mimo_ble_magic,
            {
                "magic", "dji_mimo_ble.magic",
                FT_UINT8, BASE_HEX,
            }
        },
        {
            &hf_dji_mimo_ble_length,
            {
                "msg_len", "dji_mimo_ble.length",
                FT_UINT8, BASE_HEX,
            }
        },
        {
            &hf_dji_mimo_ble_protocol_version,
            {
                "proto_ver", "dji_mimo_ble.protocol_version",
                FT_UINT8, BASE_HEX,
            }
        },
        {
            &hf_dji_mimo_ble_crc8_header,
            {
                "crc8_hdr", "dji_mimo_ble.crc8_header",
                FT_UINT8, BASE_HEX,
            }
        },
        {
            &hf_dji_mimo_ble_subsystem,
            {
                "subsystem", "dji_mimo_ble.subsystem",
                FT_UINT16, BASE_HEX,
            }
        },
        {
            &hf_dji_mimo_ble_message_id,
            {
                "msg_id", "dji_mimo_ble.message_id",
                FT_UINT16, BASE_HEX,
            }
        },
        {
            &hf_dji_mimo_ble_message_type,
            {
                "msg_type", "dji_mimo_ble.message_type",
                FT_UINT24, BASE_HEX,
            }
        },
        {
            &hf_dji_mimo_ble_payload,
            {
                "payload", "dji_mimo_ble.payload",
                FT_BYTES, BASE_NONE,
            }
        },
        {
            &hf_dji_mimo_ble_crc16_msg,
            {
                "crc16_msg", "dji_mimo_ble.crc16_msg",
                FT_UINT16, BASE_HEX,
            }
        },
    };

    static int *ett[] = {
        &ett_dji_mimo_ble
    };

    proto_dji_mimo_ble = proto_register_protocol (
                             "DJI MIMO BLE", /* protocol name        */
                             "dji_mimo_ble", /* protocol short name  */
                             "dji_mimo_ble"  /* protocol filter_name */
                         );

    proto_register_field_array(proto_dji_mimo_ble, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));

    dji_mimo_ble_handle = register_dissector_with_description (
                              "dji_mimo_ble", /* dissector name           */
                              "DJI MIMO BLE", /* dissector description    */
                              dissect_dji_mimo_ble,    /* dissector function       */
                              proto_dji_mimo_ble       /* protocol being dissected */
                          );
}

static void proto_reg_handoff_dji_mimo_ble(void)
{
    dissector_add_uint("btatt.handle", 0x002D, dji_mimo_ble_handle);
    dissector_add_uint("btatt.handle", 0x0030, dji_mimo_ble_handle);
}

WS_DLL_PUBLIC void plugin_register(void)
{
    static proto_plugin plugin_dji_mimo_ble;

    plugin_dji_mimo_ble.register_protoinfo = proto_register_dji_mimo_ble;
    plugin_dji_mimo_ble.register_handoff = proto_reg_handoff_dji_mimo_ble;
    proto_register_plugin(&plugin_dji_mimo_ble);
}

WS_DLL_PUBLIC void plugin_reg_handoff(void) {

}