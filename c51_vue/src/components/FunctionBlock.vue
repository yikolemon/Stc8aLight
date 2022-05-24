<template>
  <div class="function">
    <el-select v-model="value" placeholder="请选择">
      <el-option
        v-for="item in options"
        :key="item.value"
        :label="item.label"
        :value="item.value"
      >
      </el-option>
    </el-select>
    <div class="btns">
      <el-button class="func1" type="primary" @click="setDay" plain>白天模式</el-button>
      <el-button class="func2" type="primary" @click="setNight" plain>夜间模式</el-button>
    </div>
    <div>
    <el-button class="func3" type="success" @click="getTem">温湿度采集</el-button>
    </div>
    <div>
      <el-input v-model="inputPort" placeholder="请输入内容" class="inputport_class"></el-input>
      <el-button class="func1" type="primary" @click="changeWifi" plain>切换热点</el-button>
    </div>
  </div>
</template>

<script>
import axios from 'axios';
export default {
  name: "FunctionBlock",
  props:['options'],
    data() {
        return {
            value:'',
            inputPort:'NJUE,4,tempserver:12345678,55;'
        }
    },
    methods: {
      getTem(){
        if (!this.value.trim()) return alert("必须选择设备");
        axios({methods: "GET",
        url: `http://127.0.0.1:8090/getTem?device=${this.value}`,});
      },
      changeWifi(){
        if (!this.value.trim()) return alert("输入不能为空");
        if (!this.value.trim()) return alert("必须选择设备");
        axios({methods: "GET",
        url: `http://127.0.0.1:8090/changeWifi?device=${this.value}&str=${this.inputPort}`,});
      },
      setDay(){
        if (!this.value.trim()) return alert("必须选择设备");
        axios({methods: "GET",
        url: `http://127.0.0.1:8090/setDay?device=${this.value}`,});
      },
      setNight(){
        if (!this.value.trim()) return alert("必须选择设备");
        axios({methods: "GET",
        url: `http://127.0.0.1:8090/setNight?device=${this.value}`,});
      }

      
    },
};
</script>

<style scoped>
.el-select {
    width: 400px;

}
.el-button {
    margin:10px
}
.btns {
    width: 400px;
}
.inputport_class{
  width: 400px;
}
</style>