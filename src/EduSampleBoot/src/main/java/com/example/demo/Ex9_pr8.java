package com.example.demo;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.util.Arrays;
import java.util.List;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.servlet.ModelAndView;

@Controller
public class Ex9_pr8 {

	final static boolean test = true;

	final static String TypeBus = "Bus";
	final static String TypeFC  = "FC";
	final static String SqlSel
		= "SELECT id,shared from dep_status WHERE name=? AND type=?";
	final static String SqlIns
		= "INSERT INTO dep_status (name,type,shared) VALUES (?,?,?)";
	final static String SqlUpd
		= "UPDATE dep_status SET shared=? WHERE id=?";
	final static String SqlDel
		= "DELETE from dep_status WHERE name=? AND type=?";

	@GetMapping("/Ex9_pr8")
	public String get(@ModelAttribute FormObj fo, Model m) {
		fo.clear();
		m.addAttribute("form_obj", fo);
		m.addAttribute("msg"     , "");
		return "regDBFO.html";
	}

	@PostMapping("/Ex9_pr8")
	public ModelAndView post
		(@ModelAttribute FormObj fo, ModelAndView mv) {
		String msg = "";
		Connection con = null;
		PreparedStatement psSub = null;
		PreparedStatement ps = null;
		ResultSet rs = null;
		try {
			con = DriverManager.getConnection(DBProps.url,
												DBProps.user, DBProps.pass);
			if (fo.getName().isEmpty() || fo.getName().matches(".*[\0-/].*")) {
				msg = "名前の入力が正しくありません！";
			} else if (fo.getType().equals(TypeBus)) {
				if (! fo.getDetail().matches("[+-]?[0-9]+")) {
					msg = "利用者数の入力が正しくありません！";
				} else {
					int num = Integer.parseInt(fo.getDetail());
					psSub = con.prepareStatement(SqlSel);
					psSub.setString(1, fo.getName());
					psSub.setString(2, fo.getType());
					rs = psSub.executeQuery();
					if (test) try{Thread.sleep(num);}catch(Exception e){}
					if (! rs.next()) {
						ps = con.prepareStatement(SqlIns);
						ps.setString(1, fo.getName());
						ps.setString(2, fo.getType());
						ps.setInt(3, num);
						ps.executeUpdate();
					} else {
						if (fo.getDetail().matches("[+-].*")) {
							num += Integer.parseInt(rs.getString("shared"));
						}
						ps = con.prepareStatement(SqlUpd);
						ps.setInt(1, num);
						ps.setInt(2, rs.getInt("id"));
						ps.executeUpdate();
					}
					msg = fo.getName() + " を登録しました。";
					fo.clear();
				}
			} else if (fo.getType().equals(TypeFC)) {
				psSub = con.prepareStatement(SqlDel);
				psSub.setString(1, fo.getName());
				psSub.setString(2, fo.getType());
				psSub.executeUpdate();
				List<String> cs
					= Arrays.asList(fo.getDetail().trim().split(" +"));
				ps = con.prepareStatement(SqlIns);
				for (String c: cs) {
					if (test) try{Thread.sleep(2000);}catch(Exception e){}
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
				if (ps    != null) ps.close();
				if (psSub != null) psSub.close();
				if (con   != null) con.close();
			} catch(Exception e) {
				e.getStackTrace();
			}
		}
		mv.addObject("form_obj", fo );
		mv.addObject("msg"     , msg);
		mv.setViewName("regDBFO.html");
		return mv;
	}
}
