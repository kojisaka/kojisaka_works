package com.example.demo;

import java.util.Arrays;
import java.util.List;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;

import com.example.demo.entity.DepStatusEntity;
import com.example.demo.repository.DepStatusRepository;

import lombok.RequiredArgsConstructor;

@Controller
@RequiredArgsConstructor
public class Ex10_pr1 {

	final static boolean test = true;

	final static String TypeBus = "Bus";
	final static String TypeFC  = "FC";

	private final DepStatusRepository repository;

	@GetMapping("/Ex10_pr1")
	public String get(Model m) {
		m.addAttribute("name"  , "");
		m.addAttribute("type"  , "");
		m.addAttribute("detail", "");
		m.addAttribute("msg"   , "");
		return "regDB_JPA.html";
	}

	@PostMapping("/Ex10_pr1")
	public String post
		(@RequestParam String name,
		 @RequestParam String type,
		 @RequestParam String detail,
		 Model m) {
		String msg = "";
		try {
			if (name.isEmpty() || name.matches(".*[\0-/].*")) {
				msg = "名前の入力が正しくありません！";
			} else if (type.equals(TypeBus)) {
				if (! detail.matches("[+-]?[0-9]+")) {
					msg = "利用者数の入力が正しくありません！";
				} else {
					int num = Integer.parseInt(detail);
					List<DepStatusEntity> dl
						= repository.findByNameAndType(name, type);
					if (test) try{Thread.sleep(num);}catch(Exception e){}
					DepStatusEntity d;
					if (dl.isEmpty()) {
						d = new DepStatusEntity();
					} else {
						d = dl.get(0);
						if (detail.matches("[+-].*")) {
							num += Integer.parseInt(d.getShared());
						}
					}
					d.setBasics(name, type, String.valueOf(num));
					repository.saveAndFlush(d);
					msg = name + " を登録しました。";
					detail = type = name = "";
				}
			} else if (type.equals(TypeFC)) {
				List<String> cs
					= Arrays.asList(detail.trim().split(" +"));
				repository.deleteByNameAndType(name, type);
				for (String c: cs) {
					if (test) try{Thread.sleep(2000);}catch(Exception e){}
					DepStatusEntity d = new DepStatusEntity();
					d.setBasics(name, type, c);
					repository.save(d);
				}
				repository.flush();
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
		return "regDB_JPA.html";
	}
}
