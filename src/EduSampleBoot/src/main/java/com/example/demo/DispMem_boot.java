package com.example.demo;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.PostMapping;

@Controller
public class DispMem_boot {

	@PostMapping("/dispMem")
	public String disp() {
		return "dispMem.html";
	}
}
