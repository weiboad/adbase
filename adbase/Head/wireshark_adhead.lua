--
-- wireshark  adhead 插件使用方法
-- MAC 平台下：
--
-- 将该文件复制到/Applications/Wireshark.app/Contents/Resources/share/wireshark 目录下
-- 修改/Applications/Wireshark.app/Contents/Resources/share/wireshark/init.lua 在最后添加
-- dofile("wireshark_adhead.lua")
-- 重新启动即可
--
-- 需要注意端口号，默认是10061， 如果不是请再该文件最后修改
--
--
do
    --[[
        创建一个新的协议结构 AdHead
        第一个参数是协议名称会体现在过滤器中
        第二个参数是协议的描述信息，无关紧要
    --]]
    local adhead_proto = Proto("AdHead", "AdHead Protolcol")

    --[[
        下面定义字段
    --]]
	local head_magic = ProtoField.uint8("head.magic", "Adhead Magic", base.HEX);
    local head_datatype = ProtoField.uint8("head.datatype", "Adhead DataType", base.DEC);
    local head_mversion = ProtoField.uint8("head.mversion", "Adhead MasterVersion", base.DEC);
    local head_sversion = ProtoField.uint8("head.sversion", "Adhead SecondVersion", base.DEC);
    local head_status = ProtoField.uint16("head.status", "Adhead Status", base.DEC);
    local head_opaque = ProtoField.uint16("head.opaque", "Adhead Opaque", base.HEX);
    local head_appkey = ProtoField.uint32("head.appkey", "Adhead AppKey", base.DEC);
    local head_bodylen = ProtoField.uint32("head.bodylen", "Adhead BodyLen", base.DEC);
    local head_body = ProtoField.string("head.body", "Adhead Data", base.NONE);

    -- 将字段添加都协议中
    adhead_proto.fields = {
        head_magic,
        head_datatype,
        head_mversion,
        head_sversion,
        head_status,
        head_opaque,
        head_appkey,
        head_bodylen,
        head_body,
    }

    --[[
        下面定义 adhead 解析器的主函数，这个函数由 wireshark调用
        第一个参数是 Tvb 类型，表示的是需要此解析器解析的数据
        第二个参数是 Pinfo 类型，是协议解析树上的信息，包括 UI 上的显示
        第三个参数是 TreeItem 类型，表示上一级解析树
    --]]

	--{{{ function adhead_proto.dissector(tvb, pinfo, treeitem)

    function adhead_proto.dissector(tvb, pinfo, treeitem)

        -- 设置一些 UI 上面的信息
        pinfo.cols.protocol:set("ADHEAD")
        pinfo.cols.info:set("AdHead Protocol")

        local offset = 0
        local tvb_len = tvb:len()

        -- 在上一级解析树上创建 foo 的根节点
        local adhead_tree = treeitem:add(adhead_proto, tvb:range(offset))

        -- 下面是想该根节点上添加子节点，也就是自定义协议的各个字段
        -- 注意 range 这个方法的两个参数的意义，第一个表示此时的偏移量
        -- 第二个参数代表的是字段占用数据的长度
        adhead_tree:add(head_magic, tvb:range(offset, 1))
        offset = offset + 1
        adhead_tree:add(head_datatype, tvb:range(offset, 1))
        offset = offset + 1
        adhead_tree:add(head_mversion, tvb:range(offset, 1))
        offset = offset + 1
        adhead_tree:add(head_sversion, tvb:range(offset, 1))
        offset = offset + 1
        adhead_tree:add(head_status, tvb:range(offset, 2))
        offset = offset + 2
        adhead_tree:add(head_opque, tvb:range(offset, 2))
        offset = offset + 2
        adhead_tree:add(head_appkey, tvb:range(offset, 4))
        offset = offset + 4
        adhead_tree:add(head_bodylen, tvb:range(offset, 4))
        offset = offset + 12

        -- 计算消息内容的长度
        local head_content_len = tvb_len - offset
        adhead_tree:add(head_body, tvb:range(offset, head_content_len))
        offset = offset + head_content_len

    end

    -- 向 wireshark 注册协议插件被调用的条件
    local tcp_port_table = DissectorTable.get("tcp.port")
    tcp_port_table:add(10061, adhead_proto)
end
