package com.example.demo;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.SessionAttributes;

@Controller
@SessionAttributes("dep_st_tbl")
public class Ex9_pr3 {

	final static String TypeBus = "Bus";
	final static String TypeFC  = "FC";

	@ModelAttribute("dep_st_tbl")
	public List<DepStatus> initStorage() {
		return new ArrayList<DepStatus>();
	}

	@GetMapping("/Ex9_pr3")
	public String get(Model m) {
		m.addAttribute("name"  , "");
		m.addAttribute("type"  , "");
		m.addAttribute("detail", "");
		m.addAttribute("msg"   , "");
		return "regMem.html";
	}

	@PostMapping("/Ex9_pr3")
	public String post
		(@ModelAttribute("dep_st_tbl") List<DepStatus> dss,
		 @RequestParam String name,
		 @RequestParam String type,
		 @RequestParam String detail,
		 Model m) {
		String msg;
		try {
			if (name.isEmpty() || name.matches(".*[\0-/].*")) {
				msg = "名前の入力が正しくありません！";
			} else if (type.equals(TypeBus)) {
				if (! detail.matches("[0-9]+")) {
					msg = "利用者数の入力が正しくありません！";
				} else {
					int num = Integer.parseInt(detail);
					DepStatus ds = new BusDepStatus(name, type, num);
					dss.add(ds);
					msg = name + " を登録しました。";
					detail = type = name = "";
				}
			} else if (type.equals(TypeFC)) {
				List<String> cs
					= Arrays.asList(detail.trim().split(" +"));
				DepStatus ds = new FCDepStatus(name, type, cs);
				dss.add(ds);
				msg = name + " を登録しました。";
				detail = type = name = "";
			} else {
				msg = "業種の入力が正しくありません！";
			}
		} catch(Exception e) {
			e.getStackTrace();
			StackTraceElement tf = e.getStackTrace()[1];
			msg = "想定外の異常が発生しました！　"
					+ tf.getClassName() + "　" + tf.getMethodName()
					+ ". " + e.getMessage();
		}
		m.addAttribute("name"  , name  );
		m.addAttribute("type"  , type  );
		m.addAttribute("detail", detail);
		m.addAttribute("msg"   , msg   );
		return "regMem.html";
	}
}
