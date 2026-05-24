# -*- coding: utf-8 -*-
from libsvm.svmutil import *
from sklearn import datasets
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler

# 加载鸢尾花数据集
iris = datasets.load_iris()
X, y = iris.data, iris.target

# 划分训练集（80%）和测试集（20%）
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42, stratify=y
)

# 特征数据标准化
scaler = StandardScaler()
X_train = scaler.fit_transform(X_train)  # 训练集：拟合 + 转换
X_test = scaler.transform(X_test)        # 测试集：仅用训练集的参数转换

def convert_to_libsvm_format(X):
    """将 numpy 数组转换为 svmutil 需要的格式"""
    # 将每个样本转换为字典格式：{索引: 特征值}
    return [{idx+1: float(val) for idx, val in enumerate(row)} for row in X]

x_train_libsvm = convert_to_libsvm_format(X_train)
x_test_libsvm = convert_to_libsvm_format(X_test)

# 标签转换为列表格式
y_train_libsvm = y_train.tolist()
y_test_libsvm = y_test.tolist()

# 模型训练
# 参数说明：
# -s 0 : SVM类型 = C-SVC（多分类）
# -t 2 : 核函数类型 = RBF（径向基函数）
# -c 5 : 惩罚系数，权衡间隔大小和分类准确度
# -g 1 : RBF核的gamma参数（需根据数据调整）
# -b 1 : 开启概率估计
options = '-s 0 -t 2 -c 5 -g 1 -b 1'
model = svm_train(y_train_libsvm, x_train_libsvm, options)

# 模型预测
# p_label: 预测的标签列表
# p_acc: 预测准确率（ACC），均方误差（MSE），平方相关系数（SCC）
# p_val: 决策值或概率估计值（如果 -b 1 则返回概率）
p_label, p_acc, p_val = svm_predict(y_test_libsvm, x_test_libsvm, model, '-b 1')

# 结果分析
print("\n" + "="*40)
print("模型评估结果:")
print(f"测试集准确率: {p_acc[0]:.2f}%")
print(f"均方误差 (MSE): {p_acc[1]:.4f}")
print("="*40)