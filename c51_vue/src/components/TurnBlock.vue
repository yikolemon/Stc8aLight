<template>
  <div class="head">
    <el-row type="flex" style="align-items: center">
      <el-input v-model="input" placeholder="输入端口号" class="protIn">
      </el-input>
      <el-button class="start" @click="changePort" :disabled="disOpen"
        >启动端口</el-button
      >
      <el-button class="stop" @click="offPort" :disabled="disClose"
        >关闭端口</el-button
      >
      <span class="el-icon-success"></span>
    </el-row>
  </div>
</template>
<script>
import axios from "axios";
export default {
  name: "TurnBlock",
  props: ["add"],
  data() {
    return {
      input: "",
      disOpen: false,
      disClose: true,
    };
  },
  methods: {
    changePort() {
      if (!this.input.trim()) return alert("输入不能为空");
      // const portObj = { system: '', id:'', message: "" };
      // this.add(portObj);

      axios({
        methods: "GET",
        url: `http://127.0.0.1:8090/portOpen?port=${this.input}`,
      }).then((res) => {
         this.disOpen = true
        this.disClose = false
        if(res.data.code==200){
       
         this.$message(res.data.msg);
        }else if(res.data.code==500){
    
         this.$message(res.data.msg)
        }
      });
    },
    offPort() {
      this.disOpen = false;
      this.disClose = true;
      axios({
        methods: "GET",
        url: `http://127.0.0.1:8090/portClose`,
      }).then((res) => {
         this.disOpen = false
        this.disClose = true
        if(res.data.code==200){
       
         this.$message(res.data.msg);
        }else if(res.data.code==500){
    
         this.$message(res.data.msg)
        }
       
      });
    },
  },

};
</script>
<style scoped>
.head {
  margin-top: 30px;
}
.protIn {
  width: 200px;
  margin-left: 20%;
}
.start {
  height: 40px;
  margin-left: 10%;
  align-self: center;
}
.el-icon-success {
  display: none;
  font-size: 30px;
  color: #409eff;
  align-self: center;
}
.el-icon-success:before {
  margin: 5px;
}
</style>