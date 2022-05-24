<template>
    <el-main>
      <el-table :data="tableData">
        <el-table-column prop="system" label="系统消息" width="140">
        </el-table-column>
        <el-table-column prop="id" label="id" width="120">
        </el-table-column>
        <el-table-column prop="message" label="消息">
        </el-table-column>
      </el-table>
    </el-main>
</template>

<script>
  export default {
    name:'MessageBlock',
    props:['tableData']
  };
</script>
<style scope>
  
</style>
