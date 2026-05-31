import torch
from torch import nn
from torchvision import transforms, datasets
from torch.utils.data.dataloader import DataLoader
import torch.optim as optim
import torch.nn.functional as F
from torchinfo import summary
import os

# 继承cnn卷积神经网络类
class mixed_net(nn.Module):
    def __init__(self, num_classes=3):
        super(mixed_net, self).__init__()
        
        # 特征提取部分：卷积层 + 池化层
        # 输入: [batch, 3, 64, 64]
        self.features = nn.Sequential(
            # 第一层卷积：输入3通道，输出16通道，卷积核3x3，步长为1，外围填充1
            # 输出 [batch, 16, 64, 64]
            nn.Conv2d(3, 16, kernel_size=3, stride=1, padding=1),
            nn.BatchNorm2d(16), # 二维批归一化
            # 在每一个Batch中，将每个通道的数据拉回到均值为 0、方差为 1 的标准正态分布
            nn.ReLU(), # 激活函数
            # 最大池化：核2x2，步长2。尺寸减半 -> [batch, 16, 32, 32]
            nn.MaxPool2d(kernel_size=2, stride=2),
            
            nn.Conv2d(16, 32, kernel_size=3, stride=1, padding=1),
            nn.BatchNorm2d(32),
            nn.ReLU(),
            # 最大池化：尺寸减半 -> [batch, 32, 16, 16]
            nn.MaxPool2d(kernel_size=2, stride=2),
            
            # 第三层卷积：输入32，输出64。-> [batch, 64, 16, 16]
            nn.Conv2d(32, 64, kernel_size=3, stride=1, padding=1),
            nn.BatchNorm2d(64),
            nn.ReLU(),
            # 最大池化：尺寸减半 -> [batch, 64, 8, 8]
            nn.MaxPool2d(kernel_size=2, stride=2)
        )
        
        # 分类器部分：全连接层
        # 特征图展平后的尺寸为：64通道 * 8 * 8 = 4096
        self.classifier = nn.Sequential(
            nn.Linear(64 * 8 * 8, 512), # 全连接
            nn.ReLU(),
            nn.Dropout(0.5), # 正则化，防止过拟合
            nn.Linear(512, num_classes) # 映射到最终类别数
        )

    def forward(self, x):
        x = self.features(x)
        x = torch.flatten(x, start_dim=1) # 展平除了batch维度以外的所有维度，为全连接层做准备
        x = self.classifier(x)
        return x

if __name__ == "__main__":
    # 图像转换
    img_transforms = transforms.Compose([
        transforms.Resize([64, 64]),
        transforms.ToTensor(),
        transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
    ])
    
    # 超参数设置
    BATCH_SIZE = 1024
    EPOCH = 200

    # 加载数据
    trainset = datasets.ImageFolder(root=r'dataset/train', transform=img_transforms)
    testset1 = datasets.ImageFolder(root=r'dataset/test1', transform=img_transforms)
    testset2 = datasets.ImageFolder(root=r'dataset/test2', transform=img_transforms)

    print(f"训练集图片数量: {len(trainset)}")
    print(f"测试集1图片数量: {len(testset1)}")
    print(f"测试集2图片数量: {len(testset2)}")
    
    train_loader = DataLoader(
        trainset, 
        batch_size=BATCH_SIZE, 
        shuffle=True, 
        pin_memory=True,
        num_workers=2,  # 使用4个进程加载数据
        prefetch_factor=1  # 预加载数据
    )

    test_loader1 = DataLoader(
        testset1, 
        batch_size=BATCH_SIZE, 
        shuffle=False, 
        pin_memory=True,
        num_workers=2
    )

    test_loader2 = DataLoader(
        testset2, 
        batch_size=BATCH_SIZE, 
        shuffle=False, 
        pin_memory=True,
        num_workers=2
    )

    if torch.cuda.is_available():
        device = torch.device("cuda")
        print(f"使用设备: CUDA (GPU) - {torch.cuda.get_device_name(0)}")
    elif torch.backends.mps.is_available():
        device = torch.device("mps")
        print("使用设备: MPS (Apple Silicon GPU)")
    else:
        device = torch.device("cpu")
        print("使用设备: CPU")

    net = mixed_net().to(device)
    
    # 打印网络信息
    summary(net, input_size=(1, 3, 64, 64), device=device.type)
    print(f'标签对应的ID: {trainset.class_to_idx}')

    # 设置优化器、损失函数
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.SGD(net.parameters(), lr=0.01, momentum=0.9, weight_decay=1e-4)

    # 记录最高准确率
    max_correct = 0.0

    # 开始训练
    print("Start")
    for epoch in range(EPOCH):
        net.train() # 切换到训练模式
        train_loss = 0.0
        
        for batch_id, (datas, labels) in enumerate(train_loader):
            datas, labels = datas.to(device), labels.to(device)

            optimizer.zero_grad()
            outputs = net(datas)
            loss = criterion(outputs, labels)
            # 反向传播
            loss.backward()
            optimizer.step()

            train_loss += loss.item() * datas.size(0)

        # 计算当前 epoch 的平均训练损失
        epoch_loss = train_loss / len(train_loader.dataset)

        if epoch > 10 and (epoch + 1) % 10 == 0:
            net.eval() # 切换到评估模式
            
            correct1, correct2 = 0, 0
            total1, total2 = 0, 0

            # 分别测试两个数据集
            with torch.no_grad():
                for datas1, labels1 in test_loader1:
                    datas1, labels1 = datas1.to(device), labels1.to(device)
                    output_test1 = net(datas1)
                    _, predicted1 = torch.max(output_test1.data, dim=1)
                    total1 += labels1.size(0)
                    correct1 += (predicted1 == labels1).sum().item()

                for datas2, labels2 in test_loader2:
                    datas2, labels2 = datas2.to(device), labels2.to(device)
                    output_test2 = net(datas2)
                    _, predicted2 = torch.max(output_test2.data, dim=1)
                    total2 += labels2.size(0)
                    correct2 += (predicted2 == labels2).sum().item()

            # 计算准确率百分比
            c1 = (correct1 / total1) * 100 if total1 > 0 else 0
            c2 = (correct2 / total2) * 100 if total2 > 0 else 0
            
            print(
                f"Epoch:[{epoch + 1}/{EPOCH}]\tLoss:{epoch_loss:.5f}\t"
                f"Test1_Acc:{c1:.2f}%\tTest2_Acc:{c2:.2f}%"
            )
            
            # 定时备份临时模型
            os.makedirs("pth", exist_ok=True)
            torch.save(net.state_dict(), "pth/modeltemp.pth")

            # 比较并保存历史最佳模型（以 test1 的准确率为基准）
            if c1 > max_correct:
                max_correct = c1
                MAX_PATH = f"pth/model_best_{max_correct:.2f}.pth"
                torch.save(net.state_dict(), MAX_PATH)
                print(f"--> 发现更优模型，已保存至: {MAX_PATH}")
