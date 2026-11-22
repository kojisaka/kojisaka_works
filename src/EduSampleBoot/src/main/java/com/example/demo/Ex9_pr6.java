package com.example.demo;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.util.Arrays;
import java.util.List;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PostMapping;

@Controller
public class Ex9_pr6 {

	final static String TypeBus = "Bus";
	final static String TypeFC  = "FC";
	final static String Sql
		= "INSERT INTO dep_status (name,type,shared) VALUES (?,?,?)";

	@GetMapping("/Ex9_pr6")
	public String get(@ModelAttribute FormObj fo, Model m) {
		fo.clear();
		m.addAttribute("form_obj", fo);
		m.addAttribute("msg"     , "");
		return "regDBFO.html";
	}

	@PostMapping("/Ex9_pr6")
	public String post(@ModelAttribute FormObj fo, Model m) {
		String msg = "";
		Connection con = null;
		PreparedStatement ps = null;
		try {
			con = DriverManager.getConnection(DBProps.url,
												DBProps.user, DBProps.pass);
			if (fo.getName().isEmpty() || fo.getName().matches(".*[\0-/].*")) {
				msg = "名前の入力が正しくありません！";
			} else if (fo.getType().equals(TypeBus)) {
				if (! fo.getDetail().matches("[0-9]+")) {
					msg = "利用者数の入力が正しくありません！";
				} else {
					ps = con.prepareStatement(Sql);
					ps.setString(1, fo.getName());
					ps.setString(2, fo.getType());
					ps.setString(3, fo.getDetail());
					ps.executeUpdate();
					msg = fo.getName() + " を登録しました。";
					fo.clear();
				}
			} else if (fo.getType().equals(TypeFC)) {
				List<String> cs
					= Arrays.asList(fo.getDetail().trim().split(" +"));
				ps = con.prepareStatement(Sql);
				for (String c: cs) {
					ps.setString(1, fo.getName());
					ps.setString(2, fo.getType());
					ps.setString(3, c);
					ps.executeUpdate();
				}
				msg = fo.getName() + " を登録しました。";
				fo.clear();
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
				if (ps  != null) ps.close();
				if (con != null) con.close();
			} catch(Exception e) {
				e.getStackTrace();
			}
		}
		m.addAttribute("form_obj", fo );
		m.addAttribute("msg"     , msg);
		return "regDBFO.html";
	}
}
