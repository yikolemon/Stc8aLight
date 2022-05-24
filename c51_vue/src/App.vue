<template>
  <div id="app">
    <div class="one">
      <el-container>
        <el-header class="start">
          <TurnBlock :add="add" />
        </el-header>
        <el-container>
          <el-main class="main-message">
            <MessageBlock :tableData="tableData" />
          </el-main>
          <el-aside class="function">
            <FunctionBlock :options="options" />
          </el-aside>
        </el-container>
      </el-container>
    </div>
  </div>
</template>
  
<script>
import TurnBlock from "./components/TurnBlock.vue";
import MessageBlock from "./components/MessageBlock.vue";
import FunctionBlock from "./components/FunctionBlock.vue";

export default {
  name: "App",
  components: {
    TurnBlock,
    MessageBlock,
    FunctionBlock,
  },
  data() {
    return {
      tableData: [{}],
      options: [

      ],
    };
  },
  methods: {
    add(portObj) {
      this.tableData.unshift(portObj);
    },
    updateOptions(optionObj) {
      this.options = optionObj;
    },
  },
  mounted() {
    const _this = this;
    console.log("挂载成功！");

    //建立连接后触发
    var ws = new WebSocket("ws://127.0.0.1:8090/socket");
    ws.onopen = function () {
      console.log("建立连接成功！");
    };

    //接收到服务器的推送后触发
    ws.onmessage = function (evt) {
      var dataStr = evt.data;
      var jsonData = JSON.parse(dataStr);
      if (jsonData.systemFlag == true) {
        const portObj = { system: "true", id: "", message: jsonData.msg };
        _this.add(portObj);
      }
      else{
        const portObj = { system: "false", id: jsonData.id, message: jsonData.msg };
        _this.add(portObj);
      }
      if (jsonData.functionFlag == true) {
        const optionObj = [];

        for (var i = 0; i < jsonData.data.length; i++) {
          optionObj.unshift({
            value: jsonData.data[i],
            label: jsonData.data[i],
          });
        }
        optionObj.unshift({
          value: "All",
          label: "全部设备",
        });
        _this.updateOptions(optionObj);
      }
    };
  },
};
</script>

<style scoped>
.one {
  margin: 0px auto;
  margin-top: 10%;
  max-width: 60%;
  border-radius: 40px;
  box-shadow: 0 2px 4px rgba(20, 20, 20, 0.5);
  border-width: 1px;
}
.start {
  height: 150px !important;
}
.main-message {
  width: 50% !important;
  max-height: 500px;
  min-height: 500px;
}
.function {
  width: 50% !important;
}
.el-header {
  border: teal;
  color: #333;
  line-height: 60px;
}

.el-aside {
  color: #333;
}
</style>
