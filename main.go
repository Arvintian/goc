package main

import (
	"errors"
	"fmt"
	"math/rand"
	"os"
	"os/exec"
	"strings"
)

func getRandName(llen int) (string, error) {
	if llen < 1 {
		return "", errors.New("invalid length")
	}

	const letters = "abcdefghijklmnopqrstuvwxyz"

	// 构造随机文件名（含 /tmp/ 前缀）
	name := "/tmp/"
	for i := 0; i < llen; i++ {
		name += string(letters[rand.Intn(len(letters))])
	}

	return name, nil
}

func compile(args []string) error {
	fmt.Printf("build with args %v\n", args)
	cmd := exec.Command("gcc", args...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	if err := cmd.Run(); err != nil {
		return fmt.Errorf("compile error: %v", err)
	}
	return nil
}

func run(filename string, args []string) error {
	fmt.Printf("run with args %v\n", args)
	cmd := exec.Command(filename, args...)
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	if err := cmd.Run(); err != nil {
		return fmt.Errorf("run error: %v", err)
	}
	return nil
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: goc [build options] file1.c [run args]")
		os.Exit(1)
	}

	var buildArgs, execArgs []string
	isCFile := false

	for i := 1; i < len(os.Args); i++ {
		arg := os.Args[i]

		// 检查是否是.c文件
		if strings.HasSuffix(arg, ".c") {
			isCFile = true
			buildArgs = append(buildArgs, arg)
			continue
		}

		// 非.c文件作为运行参数
		if isCFile {
			execArgs = append(execArgs, arg)
		} else {
			if arg == "-o" {
				fmt.Println("goc can't use -o build flags")
				os.Exit(1)
			}
			buildArgs = append(buildArgs, arg)
		}
	}

	if !isCFile {
		fmt.Println("Error: No C files provided")
		os.Exit(1)
	}

	// 生成随机文件名
	exeName, err := getRandName(12)
	if err != nil {
		fmt.Printf("Error generating filename: %v\n", err)
		os.Exit(1)
	}

	// 构造编译参数
	compileArgs := append(buildArgs, "-o", exeName, "-lpthread")
	defer func() {
		// 删除临时文件
		if err := os.Remove(exeName); err != nil {
			fmt.Printf("Error removing file: %v\n", err)
			os.Exit(1)
		}
	}()

	// 编译
	if err := compile(compileArgs); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	// 运行
	if err := run(exeName, execArgs); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
}
