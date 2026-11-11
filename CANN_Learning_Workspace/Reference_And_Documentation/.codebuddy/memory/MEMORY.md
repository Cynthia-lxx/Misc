# 长期记忆：CANN_Learning_Hub 仓库

仓库 p:\Dev\CANN_Learning_Hub = 昇腾 CANN 官方开源学习中心（cann-learning-hub，GitCode: cann/cann-learning-hub）。CANN 9.0.0（2026.06.30 验证），主力教程环境 Python 3.11 + CANNLab。

## 顶层结构速查
- README.md：四大主线学习路径（①大模型推理 ~10-32h ②大模型训练 SFT/RL ③算子开发 ~20h ④推荐系统 ~8h）+ 全栈课程地图 + 高校/竞赛/SwanLab 专区。
- quick_start/：cann_basics(5课)、first_custom_operator、first_operator_api_call、first_llm_inference。
- tutorials/（13个系列）：llm_inference(Qwen3-8B 推理6课)、sft_training_pipeline(Wordle SFT 用 torchtitan-npu)、rl_training_pipeline(Wordle RL 用 verl+GRPO)、ascendc_operator_development(工程版40课)、ascendc_operator_development_light(直调版24课)、conv_operator_development(仅950)、MC2_fused_operator_development(仅950/MTE Win Area通信)、pypto_development、pyasc_operator_development、ge_development、TorchAir_development(npugraph_ex)、autofusion_development、hixl_development、hccl_development(仅slides)、CANNBot(9课pdf)。
- contrib/：torch-rechub(推荐系统6课)、swanlab_examples(mnist/医学SFT/数学LoRA)、swan_llm_course、data_structure_for_hpc。
- reference_practice/：pytorch_online_inference_operator_optimize(Qwen3.5-0.8B)、model_inference_optimization/sana_video。
- blogs/：operator~26篇 / inference~25篇 / training 3篇（真实客户实践，2025.10-2026.3）。
- skills/：ascendc-ops-project + cannjudge-submit（CANNBot 用，RSA 密文登录）。
- docs/：CANNLab 环境/课程开发指南、课程验收标准、test 分支指南。
- standard_course/：仅 README 占位（高校学时课程包规划中）。

## 约定与坑
- 教程 ipynb 惯例：xx.xx 编号、answer/、images/、src/；环境初始化导入 set_env.sh。
- 版本注意：教程大多要求 CANN 9.0.0+；Conv/MC2/TorchAir(部分) 面向 950 系列；GE/HiXL/AutoFuse/PyPTO A2/A3。
- 用户在 2026-09-03 做过一次全仓通读（见当日日志）。
