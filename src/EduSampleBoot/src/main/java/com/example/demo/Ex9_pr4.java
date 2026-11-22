package com.example.demo;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.Statement;
import java.util.Arrays;
import java.util.List;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;

@Controller
public class Ex9_pr4 {

	final static String TypeBus = "Bus";
	final static String TypeFC  = "FC";
	final static String SqlHead
		= "INSERT INTO dep_status (name,type,shared) VALUES ";

	@GetMapping("/Ex9_pr4")
	public String get(Model m) {
		m.addAttribute("name"  , "");
		m.addAttribute("type"  , "");
		m.addAttribute("detail", "");
		m.addAttribute("msg"   , "");
		return "regDB.html";
	}

	@PostMapping("/Ex9_pr4")
	public String post
		(@RequestParam String name,
		 @RequestParam String type,
		 @RequestParam String detail,
		 Model m) {
		String msg = "";
		Connection con = null;
		Statement stmt = null;
		try {
			con = DriverManager.getConnection(DBProps.url,
												DBProps.user, DBProps.pass);
			stmt = con.createStatement();
			if (name.isEmpty() || name.matches(".*[\0-/].*")) {
				msg = "名前の入力が正しくありません！";
			} else if (type.equals(TypeBus)) {
				if (! detail.matches("[0-9]+")) {
					msg = "利用者数の入力が正しくありません！";
				} else {
					String sql = SqlHead
						+ "('"	+ name + "','" + type + "','" + detail + "')";
					stmt.executeUpdate(sql);
					msg = name + " を登録しました。";
					detail = type = name = "";
				}
			} else if (type.equals(TypeFC)) {
				List<String> cs
					= Arrays.asList(detail.trim().split(" +"));
				for (String c: cs) {
					String sql = SqlHead
						+ " ('"	+ name + "','" + type + "','" + c + "')";
					stmt.executeUpdate(sql);
				}
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
		} finally {
			try {
				if (stmt != null) stmt.close();
				if (con  != null) con.close();
			} catch(Exception e) {
				e.getStackTrace();
			}
		}
		m.addAttribute("name"  , name  );
		m.addAttribute("type"  , type  );
		m.addAttribute("detail", detail);
		m.addAttribute("msg"   , msg   );
		return "regDB.html";
	}
}
