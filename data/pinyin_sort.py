#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import sys
from pypinyin import pinyin, Style

def load_stations_from_json(json_file):
    """从JSON文件加载站点信息"""
    with open(json_file, 'r', encoding='utf-8') as f:
        data = json.load(f)
    return data

def sort_stations_by_pinyin(stations):
    """按拼音对站点名称进行排序"""
    # 提取站点名称
    station_names = [station['name'] for station in stations]
    
    # 获取每个站点的拼音
    pinyin_list = []
    for name in station_names:
        # 将名称转换为拼音，使用带声调的风格
        name_pinyin = pinyin(name, style=Style.TONE)
        # 将拼音列表转换为字符串
        pinyin_str = ''.join([item[0] for item in name_pinyin])
        pinyin_list.append((name, pinyin_str))
    
    # 按拼音排序
    sorted_stations = sorted(pinyin_list, key=lambda x: x[1])
    
    # 只返回排序后的站点名称
    return [item[0] for item in sorted_stations]

def save_sorted_stations(sorted_stations, output_file):
    """保存排序后的站点到文件"""
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(sorted_stations, f, ensure_ascii=False, indent=2)

def main():
    if len(sys.argv) != 3:
        print("用法: python pinyin_sort.py <输入JSON文件> <输出JSON文件>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    # 加载数据
    data = load_stations_from_json(input_file)
    
    # 排序站点
    sorted_stations = sort_stations_by_pinyin(data['stations'])
    
    # 保存结果
    save_sorted_stations(sorted_stations, output_file)
    
    print(f"已排序 {len(sorted_stations)} 个站点，结果保存到 {output_file}")

if __name__ == "__main__":
    main()